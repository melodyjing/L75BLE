<?php

/**
 * 角色管理
 * 开发者：浮生若梦
 * 开发时间：2020/9/7
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\BaseController;
use think\facade\Request;
use think\facade\Db;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class Role extends BaseController
{
    protected $middleware = [Check::class];
    /**
     * 角色列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $page=isset($data['page'])?$data['page']:'1';
            $limit=isset($data['limit'])?$data['limit']:'10';
            $where[]=['status','<>',0];
            $count=Db::name('role')->where($where)->count();
            $roleList=Db::name('role')->where($where)->page($page,$limit)->select()->toArray();
            foreach ($roleList as $k=>$vo){
                $roleList[$k]['add_time']=date('Y-m-d',$vo['add_time']);
                $roleList[$k]['role_node']=Db::name('node')->whereIn('id',$vo['node_ids'])->where('status','<>',0)->column('node_name')?array_values(Db::name('node')->whereIn('id',$vo['node_ids'])->where('status','<>',0)->column('node_name')):[];
            }
            return json(['data'=>$roleList,'code'=>0,'count'=>$count]);
        }else{
            $edit=0;
            if(Session::get('login_user_id') == 10004){     //管理员
                $edit=1;
            }
            View::assign('edit',$edit);
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
                'role_name|角色名称' => 'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                if (Db::name('role')->where(['role_name'=>$data['role_name'],'status'=>1])->find()){
                    $ajaxarr=['code'=>400,'msg'=>'角色名称重复，请更换'];
                }else{
                    $data['add_time']=time();
                    $data['add_id']=Session::get('login_user_id');
                    $add_id=Db::name('role')->insertGetId($data);
                    if($add_id){
                        $ajaxarr=['code'=>200,'msg'=>'角色添加成功'];
                    }else{
                        $ajaxarr=['code'=>400,'msg'=>'角色添加失败'];
                    }
                }
            }
            return json($ajaxarr);
        }else {
            return View::fetch();
        }
    }

    /**
     * 角色授权
     */
    public function auth(){
        $data=request()->param();
        $id=isset($data['id'])?$data['id']:'';
        $node_ids=[];
        if($id){
            $node_ids=Db::name('role')->where(['id'=>$id])->value('node_ids')?explode(',',Db::name('role')->where(['id'=>$id])->value('node_ids')):[];
        }
        //一级节点
        $node_list = Db::name('node')->where(['pid' => 0, 'status' => 1])->field('id,node_name as title')->select()->toArray();
        foreach ($node_list as $k=>$vo){
            $children=Db::name('node')->where(['pid'=>$vo['id'],'status'=>1])->field('id,node_name as title')->select()->toArray();
            foreach ($children as $key=>$val){
                if(in_array($val['id'],$node_ids)){
                    $children[$key]['checked']='true';
                }
            }
            $node_list[$k]['children']=$children;
        }
        return json($node_list);
    }

    /**
     * 菜单编辑
     */
    public function edit(){
        $data=request()->param();
        if(request()->isAjax()){
            $validate = Validate::rule([
                'id|角色ID'=>'require',
                'role_name|角色名称' => 'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                if (Db::name('role')->where('id','<>',$data['id'])->where(['role_name'=>$data['role_name'],'status'=>1])->find()){
                    $ajaxarr=['code'=>400,'msg'=>'角色名称重复，请更换'];
                }else{
                    $data['edit_time']=time();
                    $data['edit_id']=Session::get('login_user_id');
                    $save_id=Db::name('role')->save($data);
                    if($save_id){
                        $ajaxarr=['code'=>200,'msg'=>'角色编辑成功'];
                    }else{
                        $ajaxarr=['code'=>400,'msg'=>'角色编辑失败'];
                    }
                }
            }
            return json($ajaxarr);
        }else {
            $id = isset($data['id']) ? $data['id'] : '';
            $role_data = Db::name('role')->where(['id' => $id])->field('id,role_name,desc,node_ids')->find();
            View::assign('role_data', $role_data);
            return View::fetch();
        }
    }

    /**
     * 菜单删除
     */
    public function delete(){
        $data=request()->param();
        $validate = Validate::rule([
            'id|角色ID'=>'require'
        ]);
        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            $save_id=Db::name('role')->where(['id'=>$data['id']])->save(['status'=>0,'del_time'=>time(),'del_id'=>Session::get('login_user_id')]);
            if($save_id){
                $ajaxarr=['code'=>200,'msg'=>'角色删除成功'];
            }else{
                $ajaxarr=['code'=>400,'msg'=>'角色删除失败'];
            }
        }
        return json($ajaxarr);
    }
}
