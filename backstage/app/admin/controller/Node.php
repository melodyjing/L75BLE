<?php

/**
 * 节点管理
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

class Node extends BaseController
{
    protected $middleware = [Check::class];
    /**
     * 节点列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $id=isset($data['id'])?trim($data['id']):'';
            $where[]=['status','<>',0];
            if($id){
                $where[]=['pid','=',$id];
                $nodeList=Db::name('node')->where($where)->select()->toArray();
                foreach ($nodeList as $k=>$vo){
                    $nodeList[$k]['add_time']=date('Y-m-d',$vo['add_time']);
                    $nodeList[$k]['node_pid']=Db::name('node')->where(['id'=>$vo['pid']])->value('node_name')?Db::name('node')->where(['id'=>$vo['pid']])->value('node_name'):'---';
//                    $menuList[$k]['menu_link']='/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_id']])->value('node_link').'/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_pid']])->value('node_link');
                }
            }else{
                $where[]=['pid','=','0'];
                $nodeList=Db::name('node')->where($where)->select()->toArray();
                foreach ($nodeList as $k=>$vo){
                    $nodeList[$k]['add_time']=date('Y-m-d',$vo['add_time']);
                    $nodeList[$k]['haveChild']=DB::name('node')->where(['pid'=>$vo['id']])->where('status','<>',0)->count()?true:false;
//                    $nodeList[$k]['menu_link']='/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_id']])->value('node_link').'/'.Db::name('node')->where(['status'=>1,'id'=>$vo['node_pid']])->value('node_link');
                    $nodeList[$k]['node_pid']='一级菜单';
                }
            }
            $ajaxarr=['code'=>0,'data'=>$nodeList];
            return json($ajaxarr);
        }else{
            return View::fetch();
        }
    }

    /**
     * 添加节点
     */
    public function add(){
        if(request()->isAjax()){
            $data=request()->param();
            $validate = Validate::rule([
                'node_name|节点名称' => 'require',
                'node_link|方法'=>'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                $data['add_time']=time();
                $data['add_id']=Session::get('login_user_id');
                $add_id=Db::name('node')->insertGetId($data);
                if($add_id){
                    $ajaxarr=['code'=>200,'msg'=>'节点添加成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'节点添加失败'];
                }
            }
            return json($ajaxarr);
        }else {
            //一级节点
            $node_list = Db::name('node')->where(['pid' => 0, 'status' => 1])->select();
            View::assign('node_list', $node_list);
            return View::fetch();
        }
    }

    /**
     * 节点编辑
     */
    public function edit(){
        $data=request()->param();
        if(request()->isAjax()){
            $validate = Validate::rule([
                'id|菜单ID'=>'require',
                'node_name|节点名称' => 'require',
                'node_link|方法'=>'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                $data['edit_time']=time();
                $data['edit_id']=Session::get('login_user_id');
                $save_id=Db::name('node')->save($data);
                if($save_id){
                    $ajaxarr=['code'=>200,'msg'=>'节点编辑成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'节点编辑失败'];
                }
            }
            return json($ajaxarr);
        }else {
            $id = isset($data['id']) ? $data['id'] : '';
            $node_data = Db::name('node')->where(['id' => $id])->field('id,node_name,pid,node_link')->find();
            View::assign('node_data', $node_data);
            //一级节点
            $node_list = Db::name('node')->where(['pid' => 0, 'status' => 1])->select();
            View::assign('node_list', $node_list);
            return View::fetch();
        }
    }

    /**
     * 节点删除
     */
    public function delete(){
        $data=request()->param();
        $validate = Validate::rule([
            'id|菜单ID'=>'require'
        ]);
        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            if(Db::name('node')->where(['pid'=>$data['id'],'status'=>1])->find()){
                $ajaxarr=['code'=>400,'msg'=>'此节点存在次级节点，请先删除次级节点'];
            }else{
                $save_id=Db::name('node')->where(['id'=>$data['id']])->save(['status'=>0,'del_time'=>time(),'del_id'=>Session::get('login_user_id')]);
                if($save_id){
                    $ajaxarr=['code'=>200,'msg'=>'节点删除成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'节点删除失败'];
                }
            }
        }
        return json($ajaxarr);
    }
}
