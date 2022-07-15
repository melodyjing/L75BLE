<?php

/**
 * 用户反馈
 * 开发者：浮生若梦
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\admin\model\Problem;
use app\BaseController;
use think\facade\Db;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class Problems extends BaseController
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
            $search=isset($data['search'])?trim($data['search']):'';
            $where=[];
            if($search){
                $where[]=['openid|title','like','%'.$search.'%'];
            }
            $where[]=['status','<>',0];
            $status=isset($data['status'])?$data['status']:'';
            if($status){
                $where[]=['status','=',$status];
            }
            $count=Problem::where($where)->count();
            $list = Problem::with(['Custom'])->where($where)->page($page,$limit)->order(['id'=>'desc'])->select()->toArray();
            foreach ($list as $k=>$vo){
                $list[$k]['img_src']=$vo['img_src']?explode(',',$vo['img_src']):[];
            }
            $ajaxarr=['code'=>0,'count'=>$count,'data'=>$list];
            return json($ajaxarr);
        }else{
            return View::fetch();
        }
    }

    /**
     * 处理
     */
    public function edit(){
        $data=request()->param();
        $validate = Validate::rule([
            'id|用户反馈唯一标识'=>'require'
        ]);
        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            $save_id = Problem::where(['id'=>$data['id']])->save(['status'=>2,'edit_time'=>time()]);
            if ($save_id) {
                $ajaxarr = ['code' => 200, 'msg' => '处理成功'];
            } else {
                $ajaxarr = ['code' => 400, 'msg' => '处理失败'];
            }
        }
        return json($ajaxarr);
    }
}
