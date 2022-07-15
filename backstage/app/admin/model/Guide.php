<?php
declare (strict_types = 1);

namespace app\admin\model;

use think\Model;

/**
 * @mixin \think\Model
 */
class Guide extends Model
{
    protected $autoWriteTimestamp = true;
    protected $createTime = 'add_time';
}
