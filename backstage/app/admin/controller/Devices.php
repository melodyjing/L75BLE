<?php

/**
 * 设备管理
 */

namespace app\admin\controller;

use app\admin\middleware\Check;
use app\admin\model\Custom;
use app\admin\model\Device;
use app\admin\model\Record;
use app\BaseController;
use think\facade\Db;
use think\facade\Request;
use think\facade\Session;
use think\facade\Validate;
use think\facade\View;
use think\response\Html;

class Devices extends BaseController
{
    protected $middleware = [Check::class];

    /**
     * 设备列表
     */
    public function index()
    {
        if(request()->isAjax()){
            $data = request()->param();
            $page=isset($data['page'])?$data['page']:'1';
            $limit=isset($data['limit'])?$data['limit']:'10';
            $search=isset($data['search'])?$data['search']:'';
            $where=[];
            $type=isset($data['type'])?$data['type']:'';
            if($type){
                $where[]=['type','=',$type];
            }
            if($search){
                $where[]=['mac|name','like','%'.$search.'%'];
            }
            $where[]=['status','<>',0];
            Session::set('device_where',$where);
            $count=Device::where($where)->count();
            $List=Device::with(['Custom'])->where($where)->page($page,$limit)->order(['id'=>'desc'])->select()->toArray();
            foreach ($List as $k=>$vo){
                switch($vo['type']){
                    case 1:
                        $List[$k]['type_title']='卧室';
                        break;
                    case 2:
                        $List[$k]['type_title']='客厅';
                        break;
                    case 3:
                        $List[$k]['type_title']='厨房';
                        break;
                    case 4:
                        $List[$k]['type_title']='阳台';
                        break;
                    case 5:
                        $List[$k]['type_title']='办公室';
                        break;
                    case 6:
                        $List[$k]['type_title']='其它';
                        break;
                    default:
                        $List[$k]['type_title']='';
                        break;
                }
            }
            return json(['data'=>$List,'code'=>0,'count'=>$count]);
        }else{
            return View::fetch();
        }
    }

    /**
     * 导出
     */
    public function export(){
        $data=request()->param();
        $type=isset($data['type'])?$data['type']:1;
        if($type == 1) {
            $device_id = isset($data['id']) ? $data['id'] : '';
            if ($device_id) {
                $device_data = Device::where(['id' => $device_id])->find();
                $list = Record::where(['device_id' => $device_id])->order(['year' => 'desc', 'month' => 'desc'])->select()->toArray();
                require_once(app()->getRootPath() . 'vendor/PHPExcel/PHPExcel.php');
                $objPHPExcel = new \PHPExcel();//4.激活当前的sheet表
                $objPHPExcel->setActiveSheetIndex(0);
                $objPHPExcel->getActiveSheet()->setCellValue('A1', '年');
                $objPHPExcel->getActiveSheet()->setCellValue('B1', '月');
                $objPHPExcel->getActiveSheet()->setCellValue('C1', '灭蚊数');
                foreach ($list as $k => $vo) {
                    $objPHPExcel->getActiveSheet()->setCellValue('A' . ($k + 2), $vo['year']);//客户
                    $objPHPExcel->getActiveSheet()->setCellValue('B' . ($k + 2), $vo['month']);//客户
                    $objPHPExcel->getActiveSheet()->setCellValue('C' . ($k + 2), $vo['num']);//客户
                }
                //7.设置保存的Excel表格名称
                $filename = $device_data['name'] . '-' . $device_data['mac'] . '.xls';
                //9.设置浏览器窗口下载表格
                header("Content-Type: application/force-download");
                header("Content-Type: application/octet-stream");
                header("Content-Type: application/download");
                header('Content-Disposition:inline;filename="' . $filename . '"');
                //生成excel文件
                $objWriter = \PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
                //下载文件在浏览器窗口
                $objWriter->save('php://output');
                exit;
            } else {
                return Html::create('<html><script type="text/javascript" src="/static/js/jquery-3.4.1.min.js"></script><script src="/static/layer/layer.js"></script><script>layer.msg("暂无权限",{time:2000});setTimeout(function(){parent.layer.closeAll();},2000)</script></html>', 'html', 200);
            }
        }else{
            $where=Session::get('device_where')?Session::get('device_where'):[];
            $List=Device::with(['Custom'])->where($where)->order(['id'=>'desc'])->select()->toArray();
            foreach ($List as $k=>$vo){
                switch($vo['type']){
                    case 1:
                        $List[$k]['type_title']='卧室';
                        break;
                    case 2:
                        $List[$k]['type_title']='客厅';
                        break;
                    case 3:
                        $List[$k]['type_title']='厨房';
                        break;
                    case 4:
                        $List[$k]['type_title']='阳台';
                        break;
                    case 5:
                        $List[$k]['type_title']='办公室';
                        break;
                    case 6:
                        $List[$k]['type_title']='其它';
                        break;
                    default:
                        $List[$k]['type_title']='';
                        break;
                }
                $List[$k]['num']=Record::where(['device_id'=>$vo['id']])->sum('num')?Record::where(['device_id'=>$vo['id']])->sum('num'):0;
            }
            require_once(app()->getRootPath() . 'vendor/PHPExcel/PHPExcel.php');
            $objPHPExcel = new \PHPExcel();//4.激活当前的sheet表
            $objPHPExcel->setActiveSheetIndex(0);
            $objPHPExcel->getActiveSheet()->setCellValue('A1', '微信昵称');
            $objPHPExcel->getActiveSheet()->setCellValue('B1', 'mac');
            $objPHPExcel->getActiveSheet()->setCellValue('C1', '设备类型');
            $objPHPExcel->getActiveSheet()->setCellValue('D1', '设备名称');
            $objPHPExcel->getActiveSheet()->setCellValue('E1', '设备灭蚊总数');
            $objPHPExcel->getActiveSheet()->setCellValue('F1', '添加时间');
            foreach ($List as $k => $vo) {
                $objPHPExcel->getActiveSheet()->setCellValue('A' . ($k + 2), $vo['nickName']);//客户
                $objPHPExcel->getActiveSheet()->setCellValue('B' . ($k + 2), $vo['mac']);//客户
                $objPHPExcel->getActiveSheet()->setCellValue('C' . ($k + 2), $vo['type_title']);//客户
                $objPHPExcel->getActiveSheet()->setCellValue('D' . ($k + 2), $vo['name']);//客户
                $objPHPExcel->getActiveSheet()->setCellValue('E' . ($k + 2), $vo['num']);//客户
                $objPHPExcel->getActiveSheet()->setCellValue('F' . ($k + 2), $vo['add_time']);//客户
            }
            //7.设置保存的Excel表格名称
            $filename = '设备总列表.xls';
            //9.设置浏览器窗口下载表格
            header("Content-Type: application/force-download");
            header("Content-Type: application/octet-stream");
            header("Content-Type: application/download");
            header('Content-Disposition:inline;filename="' . $filename . '"');
            //生成excel文件
            $objWriter = \PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
            //下载文件在浏览器窗口
            $objWriter->save('php://output');
            exit;
        }
    }
}
