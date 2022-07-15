<?php



namespace app\admin\controller;

use app\BaseController;
use dh2y\qrcode\QRcode;
use think\captcha\facade\Captcha;
use think\facade\Request;
use think\facade\Db;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;
use think\Image;

class Login extends BaseController
{
    /**
     * 显示登陆页面
     */
    public function index()
    {
        if(Session::has('login_user_id')){
            return redirect('/admin/index');
        }else {
            $system=Db::name('system')->where(['id'=>1])->find();
            View::assign('system',$system);
            return View::fetch();
        }
    }

    public function verify(){
        return Captcha::create();
    }

    /**
     * 登陆
     */
    public function login(){
        $data=request()->param();
        $validate = Validate::rule([
            'username|用户名' => 'require',
            'password|密码'=>'require',
            'captcha|验证码'=>'require|captcha'
        ]);

        if (!$validate->check($data)) {
            $ajaxarr = ['code' => 100, 'msg' => $validate->getError()];
        } else {
            $user_data=Db::name('user')->where(['username'=>$data['username']])->where('status','<>',0)->find();
            if($user_data){
                if($user_data['status'] == 1) {
                    if ($user_data['password'] == md5($data['password'].$user_data['salt'])) {
                        Session::set('login_user_id',$user_data['id']);
                        Db::name('login_log')->insert(['user_id'=>$user_data['id'],'login_ip'=>request()->ip(),'login_time'=>time(),'type'=>1,'title'=>'登录']);
                        $ajaxarr = ['code' => 200, 'msg' => '登陆成功'];
                    } else {
                        $ajaxarr = ['code' => 400, 'msg' => '密码错误'];
                    }
                }else{
                    $ajaxarr = ['code' => 400, 'msg' => '账户已被禁用'];
                }
            }else{
                $ajaxarr = ['code' => 400, 'msg' => '未查询到此用户'];
            }
        }
        return json($ajaxarr);
    }

    /**
     * 登出
     */
    public function logout(){
        Db::name('login_log')->insert(['user_id'=>Session::get('login_user_id'),'login_ip'=>request()->ip(),'login_time'=>time(),'type'=>2,'title'=>'退出登录']);
        Session::clear();
        return redirect('/admin/Login');
    }
}
