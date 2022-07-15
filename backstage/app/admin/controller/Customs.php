<?php

/**
 * 用户管理
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\admin\model\Custom;
use app\BaseController;
use think\facade\Db;
use think\facade\Request;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class Customs extends BaseController
{
    protected $middleware = [Check::class];

    /**
     * 用户列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $page=isset($data['page'])?$data['page']:'1';
            $limit=isset($data['limit'])?$data['limit']:'10';
            $search=isset($data['search'])?$data['search']:'';
            $where=[];
            if($search){
                $where[]=['openid|nickName','like','%'.$search.'%'];
            }
            $count=Custom::where($where)->count();
            $List=Custom::where($where)->page($page,$limit)->order('id desc')->select()->toArray();
            return json(['data'=>$List,'code'=>0,'count'=>$count]);
        }else{
            return View::fetch();
        }
    }
}
