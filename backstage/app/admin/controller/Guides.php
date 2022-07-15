<?php

/**
 * 操作指南与常见问题
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\admin\model\Guide;
use app\BaseController;
use think\facade\Request;
use think\facade\Db;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class Guides extends BaseController
{
    protected $middleware = [Check::class];
    /**
     * 列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $page=isset($data['page'])?$data['page']:'1';
            $limit=isset($data['limit'])?$data['limit']:'10';
            $where[]=['status','<>',0];
            $count=Guide::where($where)->count();
            $roleList=Guide::where($where)->page($page,$limit)->order(['sort'=>'desc','id'=>'desc'])->select()->toArray();
            return json(['data'=>$roleList,'code'=>0,'count'=>$count]);
        }else{
            return View::fetch();
        }
    }

    /**
     * 添加
     */
    public function add(){
        if(request()->isAjax()){
            $data=request()->param();
            $validate = Validate::rule([
                'title|标题' => 'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                unset($data['file']);
                $data['add_time']=time();
                $add_id=Guide::insertGetId($data);
                if($add_id){
                    $ajaxarr=['code'=>200,'msg'=>'添加成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'添加失败'];
                }
            }
            return json($ajaxarr);
        }else {
            return View::fetch();
        }
    }

    /**
     * 编辑
     */
    public function edit(){
        $data=request()->param();
        if(request()->isAjax()){
            $validate = Validate::rule([
                'id|ID'=>'require',
                'title|标题' => 'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                unset($data['file']);
                $data['edit_time']=time();
                $save_id=Guide::where(['id'=>$data['id']])->save($data);
                if($save_id){
                    $ajaxarr=['code'=>200,'msg'=>'编辑成功'];
                }else{
                    $ajaxarr=['code'=>400,'msg'=>'编辑失败'];
                }
            }
            return json($ajaxarr);
        }else {
            $id = isset($data['id']) ? $data['id'] : '';
            $result_data = Guide::where(['id' => $id])->find();
            View::assign('result_data', $result_data);
            return View::fetch();
        }
    }

    /**
     * 删除
     */
    public function delete(){
        $data=request()->param();
        $validate = Validate::rule([
            'id|ID'=>'require'
        ]);
        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            $save_id=Guide::where(['id'=>$data['id']])->save(['status'=>0,'del_time'=>time()]);
            if($save_id){
                $ajaxarr=['code'=>200,'msg'=>'删除成功'];
            }else{
                $ajaxarr=['code'=>400,'msg'=>'删除失败'];
            }
        }
        return json($ajaxarr);
    }
}
