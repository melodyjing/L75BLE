<?php

/**
 * 账户管理
 * 开发者：浮生若梦
 * 开发时间：2020/9/7
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\BaseController;
use think\facade\Db;
use think\facade\Filesystem;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class User extends BaseController
{
    protected $middleware = [Check::class];
    /**
     * 菜单列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $page=isset($data['page'])?$data['page']:'1';
            $limit=isset($data['limit'])?$data['limit']:'10';
            $search=isset($data['search'])?trim($data['search']):'';
            $where=[];
            if($search){
                $where[]=['username|truename','like','%'.$search.'%'];
            }
            $where[]=['status','<>',0];

            $login_role_id=Db::name('user')->where(['id' => Session::get('login_user_id')])->value('role_id');
            if($login_role_id != 1) {
                $where[]=['id','=',Session::get('login_user_id')];
            }
            $count= Db::name('user')->where($where)->count();
            $userList = Db::name('user')->where($where)->page($page,$limit)->select()->toArray();
            foreach ($userList as $k => $vo) {
                $userList[$k]['add_time'] = date('Y-m-d', $vo['add_time']);
                $userList[$k]['role_name'] = DB::name('role')->where(['id' => $vo['role_id']])->value('role_name') ? DB::name('role')->where(['id' => $vo['role_id']])->value('role_name') : '';
            }
            $ajaxarr=['code'=>0,'count'=>$count,'data'=>$userList];
            return json($ajaxarr);
        }else{
            $is_show=0;
            $login_role_id=Db::name('user')->where(['id' => Session::get('login_user_id')])->value('role_id');
            if($login_role_id == 1) {
                $is_show=1;
            }
            View::assign('is_show',$is_show);
            return View::fetch();
        }
    }

    /**
     * 添加菜单
     */
    public function add(){
        if(request()->isAjax()){
            $data=request()->param();
            $validate = Validate::rule([
                'username|账户名称' => 'require',
                'truename|真实姓名' => 'require',
                'avatar|头像' => 'require',
                'password|密码' => 'require',
                'role_id|所属角色' => 'require',
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                if (Db::name('user')->where(['username'=>$data['username']])->where('status','<>',0)->find()){
                    $ajaxarr=['code'=>400,'msg'=>'账户名称重复，请更换'];
                }else{
                    unset($data['file']);
                    $data['salt']=$this->getRandomString(6);
                    $data['password']=md5($data['password'].$data['salt']);
                    $data['add_time']=time();
                    $data['add_id']=Session::get('login_user_id');
                    $add_id=Db::name('user')->insertGetId($data);
                    if($add_id){
                        $ajaxarr=['code'=>200,'msg'=>'账户添加成功'];
                    }else{
                        $ajaxarr=['code'=>400,'msg'=>'账户添加失败'];
                    }
                }
            }
            return json($ajaxarr);
        }else {
            $role_list=Db::name('role')->where('status','<>',0)->select();
            View::assign('role_list',$role_list);
            return View::fetch();
        }
    }

    /**
     * 上传
     */
    public function upload(){
        $file = request()->file('file');
        if(!Validate::fileSize($file,1024 * 1024 * 5)){
            $ajaxarr=['code'=>400,'msg'=>'图片过大'];
        }else if(!Validate::fileExt($file,'jpeg,jpg,png,gif')){
            $ajaxarr=['code'=>400,'msg'=>'图片格式错误'];
        }else{
            $info = Filesystem::disk('public')->putFile('upload/avatar',$file);
            $img_path='/storage/'.$info;
            $ajaxarr=['code'=>0,'data'=>['src'=>$img_path]];
        }
        return json($ajaxarr);
    }

    /**
     * 菜单编辑
     */
    public function edit(){
        $data=request()->param();
        if(request()->isAjax()){
            $validate = Validate::rule([
                'username|账户名称' => 'require',
                'truename|真实姓名' => 'require',
                'avatar|头像' => 'require',
                'role_id|所属角色' => 'require',
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
            $id = isset($data['id']) ? $data['id'] : '';
            $user_data = Db::name('user')->where(['id' => $id])->find();
            View::assign('user_data', $user_data);
            $role_list=Db::name('role')->where('status','<>',0)->select();
            View::assign('role_list',$role_list);
            return View::fetch();
        }
    }

    /**
     * 菜单删除
     */
    public function delete(){
        $data=request()->param();
        $validate = Validate::rule([
            'id|账户ID'=>'require'
        ]);
        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            $type=isset($data['type'])?$data['type']:'1';
            if($type == 0){
                $save_id=Db::name('user')->where(['id'=>$data['id']])->save(['status'=>0,'del_time'=>time(),'del_id'=>Session::get('login_user_id')]);
                if($save_id){
                    $ajaxarr=['code'=>200,'msg'=>'账户删除成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'账户删除失败'];
                }
            }else{
                $save_id=Db::name('user')->where(['id'=>$data['id']])->save(['status'=>$type,'edit_time'=>time(),'edit_id'=>Session::get('login_user_id')]);
                if($save_id){
                    $ajaxarr=['code'=>200,'msg'=>'账户状态修改成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'账户状态修改失败'];
                }
            }
        }
        return json($ajaxarr);
    }

    function getRandomString($len, $chars=null)
    {
        if (is_null($chars)) {
            $chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        }
        mt_srand(10000000*(double)microtime());
        for ($i = 0, $str = '', $lc = strlen($chars)-1; $i < $len; $i++) {
            $str .= $chars[mt_rand(0, $lc)];
        }
        return $str;
    }
}
