<?php
/**
 * 基础设置
 * 开发者：浮生若梦
 * 开发时间：2020/2/3
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\BaseController;
use think\facade\Db;
use think\facade\Request;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;

class Config extends BaseController
{
    protected $middleware = [Check::class];

    /**
     * 编辑
     */
    public function edit(){
        $data=request()->param();
        if(request()->isAjax()){
            $validate = Validate::rule([
                'appid|小程序appid'=>'require',
                'appsecret|小程序appsecret'=>'require'
            ]);
            if (!$validate->check($data)) {
                $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
            } else {
                unset($data['file']);
                $save_id = Db::name('wechat_config')->where(['id'=>1])->save($data);
                if ($save_id) {
                    Db::name('wechat_token')->delete(true);
                    $ajaxarr = ['code' => 200, 'msg' => '配置成功'];
                } else {
                    $ajaxarr = ['code' => 400, 'msg' => '配置失败'];
                }
            }
            return json($ajaxarr);
        }else {
            $config = Db::name('wechat_config')->where(['id' => 1])->find();
            View::assign('config', $config);
            return View::fetch();
        }
    }
}
