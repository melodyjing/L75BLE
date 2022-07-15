<?php
// 应用公共文件

/**
 * 内存单位
 * $filesize 为 filesize(文件)获取的字节数
 * 例如 filesize(substr(/storage/upload/avatar/20200913/95dd099bbf58eb4dceeb9a6e76f22ff7.png, 1))
 */
function getSize($filesize)
{
    if ($filesize >= 1073741824) {
        $filesize = round($filesize / 1073741824 * 100) / 100 . ' GB';
    } elseif ($filesize >= 1048576) {
        $filesize = round($filesize / 1048576 * 100) / 100 . ' MB';
    } elseif ($filesize >= 1024) {
        $filesize = round($filesize / 1024 * 100) / 100 . ' KB';
    } else {
        $filesize = $filesize . ' 字节';
    }
    return $filesize;
}

/**
 * 生成几位字符  默认6位
 */
function getRandomString($len=6,$format='ALL') {
    switch($format) {
        case 'ALL':
            $chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-@#~'; break;
        case 'CHAR':
            $chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-@#~'; break;
        case 'NUMBER':
            $chars='0123456789'; break;
        default :
            $chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-@#~';
            break;
    }
    mt_srand((double)microtime()*1000000*getmypid());
    $password="";
    while(strlen($password)<$len)
        $password.=substr($chars,(mt_rand()%strlen($chars)),1);
    return $password;
}

/**
 * 获取年龄
 * @param $birthday 出生年月日（1992-1-3）
 * @return string 年龄
 */
function countage($birthday){
    $year=date('Y');
    $month=date('m');
    if(substr($month,0,1)==0){
        $month=substr($month,1);
    }
    $day=date('d');
    if(substr($day,0,1)==0){
        $day=substr($day,1);
    }
    $arr=explode('-',$birthday);
    $age=$year-$arr[0];
    if($month<$arr[1]){
        $age=$age-1;

    }elseif($month==$arr[1]&&$day<$arr[2]){
        $age=$age-1;

    }
    return $age;
}

/**
 * 发送请求
 * @param $url              请求url
 * @param string $data      请求数据
 * @param string $method    请求方式
 * @return bool|string
 */
function httpRequest($url, $data = '', $method = 'GET')
{
    $curl = curl_init();
    curl_setopt($curl, CURLOPT_URL, $url);
    curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_setopt($curl, CURLOPT_USERAGENT, $_SERVER['HTTP_USER_AGENT']);
    curl_setopt($curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($curl, CURLOPT_AUTOREFERER, 1);
    if ($method == 'POST') {
        curl_setopt($curl, CURLOPT_POST, 1);
        if ($data != '') {
            curl_setopt($curl, CURLOPT_POSTFIELDS, $data);
        }
    }

    curl_setopt($curl, CURLOPT_TIMEOUT, 30);
    curl_setopt($curl, CURLOPT_HEADER, 0);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
    $result = curl_exec($curl);
    curl_close($curl);
    return $result;
}
