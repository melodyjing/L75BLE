<?php
declare (strict_types = 1);

namespace app\admin\model;

use think\Model;

/**
 * @mixin \think\Model
 */
class Device extends Model
{
    protected $autoWriteTimestamp = true;
    protected $createTime = 'add_time';

    /**
     * 获取对应用户
     */
    public function Custom(){
        return $this->hasOne(Custom::class,'openid','openid')->bind(['nickName','avatar']);
    }
}
