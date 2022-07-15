<?php

namespace app\admin\controller;

use app\BaseController;
use think\facade\Db;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;
use wechat_pay\WechatPay;

class Index extends BaseController
{
    public function initialize()
    {
        parent::initialize(); // TODO: Change the autogenerated stub
        $this->user_id=Session::get('login_user_id');
    }

    public function test(){
        $wechat_pay=new WechatPay();
        $description='演示微信h5支付';
        $out_trade_no=$wechat_pay->order_number();
        $notify_url='http://47.100.191.178/index/Index/index';
        $amount=['total'=>'10'];
        $body=['description'=>$description,'out_trade_no'=>$out_trade_no,'notify_url'=>$notify_url,'amount'=>$amount];
        $res=$wechat_pay->h5_unified_order($body);
        halt($res);
    }

    /**
     * 显示资源列表
     */
    public function index()
    {
        if(Session::has('login_user_id')){
            //根据登录账户对应角色权限展示菜单
            $role_id = Db::name('user')->where(['id' => Session::get('login_user_id')])->value('role_id');
            $node_ids = Db::name('role')->where(['id' => $role_id, 'status' => 1])->value('node_ids') ? explode(',', Db::name('role')->where(['id' => $role_id, 'status' => 1])->value('node_ids')) : [];

            $menu_ids=[];
            //获取所有可显示菜单
            $menu_list=Db::name('menu')->where(['status'=>1])->where(['pid'=>0])->field('id,pid,node_id,node_pid')->select()->toArray();
            foreach ($menu_list as $key=>$val){
                $sub_menu=Db::name('menu')->where(['status'=>1,'pid'=>$val['id']])->select()->toArray();
                if($sub_menu){   //存在二级菜单
                    foreach ($sub_menu as $k=>$vo){
                        if(in_array($vo['node_id'],$node_ids) && in_array($vo['node_pid'],$node_ids)){
                            $menu_ids[]=$vo['pid'];
                            $menu_ids[]=$vo['id'];
                        }
                    }
                }else{
                    if(in_array($val['node_id'],$node_ids) && in_array($val['node_pid'],$node_ids)){
                        $menu_ids[]=$val['id'];
                    }
                }
            }

            //得到所有可显示菜单
            $menu_list=Db::name('menu')->where(['status'=>1,'pid'=>0])->whereIn('id',$menu_ids)->order('sort desc')->select()->toArray();
            foreach ($menu_list as $k=>$vo){
                $sub_menu=Db::name('menu')->where(['status'=>1,'pid'=>$vo['id']])->whereIn('id',$menu_ids)->order('sort desc')->select()->toArray();
                if(count($sub_menu) > 0){
                    $menu_list[$k]['sub_menu_num']=1;
                }else{
                    $menu_list[$k]['sub_menu_num']=0;
                }
                $menu_list[$k]['menu_link']='/admin/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_id']])->value('node_link').'/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_pid']])->value('node_link');

                foreach ($sub_menu as $key=>$val){
                    $sub_menu[$key]['menu_link']='/admin/'.Db::name('node')->where(['status'=>1,'id'=>$val['node_id']])->value('node_link').'/'.Db::name('node')->where(['status'=>1,'id'=>$val['node_pid']])->value('node_link');
                }
                $menu_list[$k]['sub_menu']=$sub_menu;
            }
            View::assign('menu_list',$menu_list);


            $user_data=Db::name('user')->where(['id'=>$this->user_id])->find();
            View::assign('user_data',$user_data);
            View::assign('year',date('Y'));
            $system=Db::name('system')->where(['id'=>1])->find();
            View::assign('system',$system);

            return View::fetch();
        }else {
            return redirect('/admin/Login');
        }
    }

    /**
     * 编辑资料
     */
    public function edit_mater(){
        if(request()->isAjax()){
            $data=request()->param();
            $validate = Validate::rule([
                'id|账户ID'=>'require',
                'username|账户名称' => 'require',
                'avatar|头像' => 'require',
                'email|邮箱' => 'require',
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                if (Db::name('user')->where('id','<>',$data['id'])->where(['username'=>$data['username']])->where('status','<>',0)->find()){
                    $ajaxarr=['code'=>400,'msg'=>'账户名称重复，请更换'];
                }else{
                    $password=isset($data['password'])?$data['password']:'';
                    if($password){
                        $data['salt']=$this->getRandomString(6);
                        $data['password']=md5($password.$data['salt']);
                    }else{
                        unset($data['password']);
                    }
                    unset($data['file']);
                    $data['edit_time']=time();
                    $data['edit_id']=Session::get('login_user_id');
                    $save_id=Db::name('user')->save($data);
                    if($save_id){
                        $ajaxarr=['code'=>200,'msg'=>'账户编辑成功'];
                    }else{
                        $ajaxarr=['code'=>400,'msg'=>'账户编辑失败'];
                    }
                }
            }
            return json($ajaxarr);
        }else {
            $user_id = Session::get('login_user_id');
            $user_data=Db::name('user')->where(['id'=>$user_id])->find();
            View::assign('user_data',$user_data);
            $role_list=Db::name('role')->where('status','<>',0)->select();
            View::assign('role_list',$role_list);
            return View::fetch();
        }
    }

    //随机产生六位数密码Begin
    function getRandomString($len=6,$format='ALL') {
        switch($format) {
            case 'ALL':
                $chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-@#~'; break;
            case 'CHAR':
                $chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-@#~'; break;
            case 'NUMBER':
                $chars='0123456789'; break;
            default :
                $chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-@#~';
                break;
        }
        mt_srand((double)microtime()*1000000*getmypid());
        $password="";
        while(strlen($password)<$len)
            $password.=substr($chars,(mt_rand()%strlen($chars)),1);
        return $password;
    }
}

