<?php

/**
 * 菜单管理
 * 开发者：浮生若梦
 * 开发时间：2020/9/1
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\BaseController;
use think\facade\Db;
use think\facade\Request;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class Menu extends BaseController
{
    protected $middleware = [Check::class];
    /**
     * 菜单列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $id=isset($data['id'])?trim($data['id']):'';
            $menu_name=isset($data['menu_name'])?trim($data['menu_name']):'';
            $where=[];
            if($menu_name){
                $where[]=['menu_name','like','%'.$menu_name.'%'];
            }
            $where[]=['status','<>',0];
            if($id){
                $where[]=['pid','=',$id];
                $menuList=Db::name('menu')->where($where)->order('sort desc')->select()->toArray();
                foreach ($menuList as $k=>$vo){
                    $menuList[$k]['add_time']=date('Y-m-d',$vo['add_time']);
                    $menuList[$k]['menu_pid']=Db::name('menu')->where(['id'=>$vo['pid']])->value('menu_name')?Db::name('menu')->where(['id'=>$vo['pid']])->value('menu_name'):'---';
                    $menuList[$k]['menu_link']='/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_id']])->value('node_link').'/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_pid']])->value('node_link');
                }
            }else{
                $where[]=['pid','=','0'];
                $menuList=Db::name('menu')->where($where)->order('sort desc')->select()->toArray();
                foreach ($menuList as $k=>$vo){
                    $menuList[$k]['add_time']=date('Y-m-d',$vo['add_time']);
                    $menuList[$k]['haveChild']=DB::name('menu')->where(['pid'=>$vo['id']])->where('status','<>',0)->count()?true:false;
                    $menuList[$k]['menu_link']='/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_id']])->value('node_link').'/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_pid']])->value('node_link');
                    $menuList[$k]['menu_pid']='一级菜单';
                }
            }
            $ajaxarr=['code'=>0,'data'=>$menuList];
            return json($ajaxarr);
        }else{
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
                'menu_name|菜单名称' => 'require',
                'node_id|一级节点' => 'require',
                'node_pid|二级节点' => 'require',
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                $pid=isset($data['pid'])?$data['pid']:0;
                $menu_class=isset($data['menu_class'])?$data['menu_class']:'';
                if($pid == 0 && $menu_class == ''){
                    $ajaxarr=['code'=>100,'msg'=>'请选择正确的菜单图标'];
                }elseif (Db::name('menu')->where(['menu_name'=>$data['menu_name'],'status'=>1])->find()){
                    $ajaxarr=['code'=>400,'msg'=>'菜单名称重复，请更换'];
                }else{
                    $data['add_time']=time();
                    $data['add_id']=Session::get('login_user_id');
                    $add_id=Db::name('menu')->insertGetId($data);
                    if($add_id){
                        $ajaxarr=['code'=>200,'msg'=>'菜单添加成功'];
                    }else{
                        $ajaxarr=['code'=>400,'msg'=>'菜单添加失败'];
                    }
                }
            }
            return json($ajaxarr);
        }else {
            //菜单
            $menu_list = Db::name('menu')->where(['pid' => 0, 'status' => 1])->order('sort desc')->select();
            View::assign('menu_list', $menu_list);
            //一级节点
            $node_list = Db::name('node')->where(['pid' => 0, 'status' => 1])->select();
            View::assign('node_list', $node_list);
            return View::fetch();
        }
    }

    /**
     * 节点选择
     */
    public function sub_node(){
        $data=request()->param();
        $node_id=isset($data['node_id'])?trim($data['node_id']):'';
        if($node_id){
            $sub_node_list=Db::name('node')->where(['pid'=>$node_id,'status'=>1])->select();
            $ajaxarr=['code'=>200,'sub_node_list'=>$sub_node_list];
        }else{
            $ajaxarr=['code'=>100,'msg'=>'参数一级节点id丢失'];
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
                'id|菜单ID'=>'require',
                'menu_name|菜单名称' => 'require',
                'node_id|一级节点' => 'require',
                'node_pid|二级节点' => 'require',
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                $pid=isset($data['pid'])?$data['pid']:0;
                $menu_class=isset($data['menu_class'])?$data['menu_class']:'';
                if($pid == 0 && $menu_class == ''){
                    $ajaxarr=['code'=>100,'msg'=>'请选择正确的菜单图标'];
                }elseif (Db::name('menu')->where('id','<>',$data['id'])->where(['menu_name'=>$data['menu_name'],'status'=>1])->find()){
                    $ajaxarr=['code'=>400,'msg'=>'菜单名称重复，请更换'];
                }else{
                    $data['edit_time']=time();
                    $data['edit_id']=Session::get('login_user_id');
                    $save_id=Db::name('menu')->save($data);
                    if($save_id){
                        $ajaxarr=['code'=>200,'msg'=>'菜单编辑成功'];
                    }else{
                        $ajaxarr=['code'=>400,'msg'=>'菜单编辑失败'];
                    }
                }
            }
            return json($ajaxarr);
        }else {
            $id = isset($data['id']) ? $data['id'] : '';
            $menu_data = Db::name('menu')->where(['id' => $id])->field('id,menu_name,pid,menu_class,node_id,node_pid,sort')->find();
            View::assign('menu_data', $menu_data);
            //菜单
            $menu_list = Db::name('menu')->where(['pid' => 0, 'status' => 1])->order('sort desc')->select();
            View::assign('menu_list', $menu_list);
            //一级节点
            $node_list = Db::name('node')->where(['pid' => 0, 'status' => 1])->select();
            View::assign('node_list', $node_list);
            //二级节点
            $sub_node_list = Db::name('node')->where(['pid' => $menu_data['node_id'], 'status' => 1])->select();
            View::assign('sub_node_list', $sub_node_list);
            return View::fetch();
        }
    }

    /**
     * 菜单删除
     */
    public function delete(){
        $data=request()->param();
        $validate = Validate::rule([
            'id|菜单ID'=>'require'
        ]);
        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            if(Db::name('menu')->where(['pid'=>$data['id'],'status'=>1])->find()){
                $ajaxarr=['code'=>400,'msg'=>'此菜单存在二级菜单，请先删除二级菜单'];
            }else{
                $save_id=Db::name('menu')->where(['id'=>$data['id']])->save(['status'=>0,'del_time'=>time(),'del_id'=>Session::get('login_user_id')]);
                if($save_id){
                    $ajaxarr=['code'=>200,'msg'=>'菜单删除成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'菜单删除失败'];
                }
            }
        }
        return json($ajaxarr);
    }

}
