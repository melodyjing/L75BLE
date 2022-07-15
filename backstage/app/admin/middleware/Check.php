<?php
/**
 * 请求中间件
 */

namespace app\admin\middleware;

use think\facade\Db;
use think\facade\Session;
use think\response\Html;

class Check
{

    public function handle($request, \Closure $next)
    {
        if (Session::has('login_user_id') && Session::get('login_user_id') != '') {
            //判断权限
            $role_id = Db::name('user')->where(['id' => Session::get('login_user_id')])->value('role_id');
            $node_id = Db::name('node')->where(['node_link' => request()->controller(), 'status' => 1])->value('id');
            $node_pid = Db::name('node')->where(['node_link' => request()->action(), 'pid' => $node_id, 'status' => 1])->value('id');
            $node_ids = Db::name('role')->where(['id' => $role_id, 'status' => 1])->value('node_ids') ? explode(',', Db::name('role')->where(['id' => $role_id, 'status' => 1])->value('node_ids')) : [];

            if (in_array($node_id, $node_ids) && in_array($node_pid, $node_ids)) {
                return $next($request);
            } else {
                if (request()->isAjax()) {
                    return json(['code'=>400,'msg'=>'暂无权限']);
                } else {
                    return Html::create('<html><script type="text/javascript" src="/static/js/jquery-3.4.1.min.js"></script><script src="/static/layer/layer.js"></script><script>layer.msg("暂无权限",{time:2000});setTimeout(function(){parent.layer.closeAll();},2000)</script></html>','html',200);
                }
            }
        } else {
            if (request()->isAjax()) {
                return json(['code'=>400,'msg'=>'登录已失效，请重新登录']);
//                return Html::create('<html><script type="text/javascript" src="/static/js/jquery-3.4.1.min.js"></script><script src="/static/layer/layer.js"></script><script>layer.msg("登录失效，请刷新重新登录",{time:2000});setTimeout(function(){ top.location.href="/admin/Login/index";},1000)</script></html>','html',200);
            } else {
                return Html::create('<html><script type="text/javascript" src="/static/js/jquery-3.4.1.min.js"></script><script src="/static/layer/layer.js"></script><script>layer.msg("登录失效，请刷新重新登录",{time:2000});setTimeout(function(){ top.location.href="/admin/Login/index";},1000)</script></html>','html',200);
            }
        }
    }
}

