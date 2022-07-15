<?php
/**
 * 系统配置
 * 开发者：浮生若梦
 * 开发时间：2020/11/30
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\BaseController;
use think\facade\Db;
use think\facade\Request;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class System extends BaseController
{
    protected $middleware = [Check::class];

    /**
     * 编辑
     */
    public function edit(){
        $data=request()->param();
        if(request()->isAjax()){
            $validate = Validate::rule([
                'title|系统后台标题' => 'require',
                'login_src|登录背景图'=>'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                unset($data['file']);
                $data['edit_time']=time();
                $data['edit_id']=Session::get('login_user_id');
                $save_id = Db::name('system')->where(['id'=>1])->save($data);
                if ($save_id) {
                    $ajaxarr = ['code' => 200, 'msg' => '编辑成功'];
                } else {
                    $ajaxarr = ['code' => 400, 'msg' => '编辑失败'];
                }
            }
            return json($ajaxarr);
        }else {
            $config = Db::name('system')->where(['id' => 1])->find();
            View::assign('config', $config);
            return View::fetch();
        }
    }
}
