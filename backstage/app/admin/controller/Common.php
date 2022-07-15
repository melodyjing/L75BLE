<?php

/**
 * 公共模块
 */

namespace app\admin\controller;

use app\BaseController;
use think\facade\Filesystem;
use think\facade\Validate;
use think\facade\Request;

class Common extends BaseController
{
    /**
     * 图片的公共上传
     */
    public function upload(){
        $data=request()->param();
        $folder=isset($data['folder'])?$data['folder']:'avatar';
        $file = request()->file('file');
        if(!Validate::fileSize($file,1024 * 1024 * 2)){
            $ajaxarr=['code'=>400,'msg'=>'图片过大,请勿超过2M'];
        }else if(!Validate::fileExt($file,'jpeg,jpg,png,gif,ico')){
            $ajaxarr=['code'=>400,'msg'=>'图片格式错误'];
        }else{
            $info = Filesystem::disk('public')->putFile('upload/'.$folder,$file);
            $img_path='/storage/'.$info;
            $ajaxarr=['code'=>0,'data'=>['src'=>$img_path]];
        }
        return json($ajaxarr);
    }
    /**
     * 文件的公共上传
     */
    public function upload_file(){
        $data=request()->param();
        $folder=isset($data['folder'])?$data['folder']:'avatar';
        $file = request()->file('file');
        if(!Validate::fileSize($file,1024 * 1024 * 5)){
            $ajaxarr=['code'=>400,'msg'=>'文件过大,请勿超过5M'];
        }else{
            $info = Filesystem::disk('public')->putFile('upload/'.$folder,$file);
            $img_path='/storage/'.$info;
            $ajaxarr=['code'=>0,'data'=>['src'=>$img_path]];
        }
        return json($ajaxarr);
    }
}
