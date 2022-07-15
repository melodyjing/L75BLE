// pages/device/info.js
import * as echarts from '../../ec-canvas/echarts';

const app = getApp()

var chart = null;
var option = {
  grid: {
    containLabel: true,
    show: true,
    left: '3%',
    right: '3%',
    bottom: '0',
    top: '40',
    borderWidth: '0',
  },
  dataZoom: [{
    type: 'inside', //滑动轴
    start: 1, //距离左侧0刻度的距离，1%
    end: 45 //距离左侧0刻度的距离，35%，相当于规定了滑动的范围
  }],
  xAxis: {
    type: 'category',
    boundaryGap: false,
    data: ['1月', '2月', '3月', '4月', '5月', '6月', '7月', '8月', '9月', '10月', '11月', '12月'],
    splitLine: {
      show: true,
    },
    axisLine: { //y轴
      show: false
    },
    axisTick: { //y轴刻度线
      show: false
    },
    axisLabel: {
      fontSize: 20,
      fontWeight: 500,
      align: 'center',
    }
  },
  yAxis: {
    x: 'center',
    type: 'value',
    splitLine: {
      show: false,
    },
    axisLine: { //y轴
      show: false
    },
    axisTick: { //y轴刻度线
      show: false
    },
    axisLabel: {
      formatter: function () {
        return "";
      }
    }
  },
  series: [{
    type: 'line',
    smooth: true,
    label: {
      show: true,
      position: 'top',
      fontSize: 20,
    },
    areaStyle: {
      color: 'rgba(20,76,247,0.2)'
    },
    data: []
    // data: [30, 15, 20, 30, 10, 14, 2, 4, 1, 5, 6, 2]
  }]
};

function initChart(canvas, width, height, dpr) {
  chart = echarts.init(canvas, null, {
    width: width * 2,
    height: height * 2,
    devicePixelRatio: dpr // new
  });
  canvas.setChart(chart);
  chart.setOption(option);
  return chart;
}

Page({

  /**
   * 页面的初始数据
   */
  data: {
    statusBarHeight: app.globalData.statusBarHeight,
    navHeight: app.globalData.navHeight,
    set_list: {
      1: {
        id: 1,
        title: '周一',
        checked: false
      },
      2: {
        id: 2,
        title: '周二',
        checked: false
      },
      3: {
        id: 3,
        title: '周三',
        checked: false
      },
      4: {
        id: 4,
        title: '周四',
        checked: false
      },
      5: {
        id: 5,
        title: '周五',
        checked: false
      },
      6: {
        id: 6,
        title: '周六',
        checked: false
      },
      7: {
        id: 7,
        title: '周日',
        checked: false
      }
    },
    device_list: [],
    ec: {
      onInit: initChart
    },
    year: new Date().getFullYear(),
    device_year: new Date().getFullYear(),
    show_share: false,
    shareImagePath: '',
    tip: '',
    fail_show: false,
    show_list: false,
    open_click: 0,
    year_click: 0,
    year_bottom: 0,
    chartOption: option,

    width: wx.getSystemInfoSync().windowWidth,
    window_height: wx.getSystemInfoSync().windowHeight,
    height: 0,
    mysear: true, //停止搜索用 
    serviceId: '0000FFB0-0000-1000-8000-00805F9B34FB',
    notify_id: '0000FFB2-0000-1000-8000-00805F9B34FB',
    write_id: '0000FFB1-0000-1000-8000-00805F9B34FB',

    Is_OK_NO: true,
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    var id = options.id;
    that.setData({
      id: id
    })
    that.data.isFirst = true
  },
  onShow() {
    var that = this;
    if (that.data.device_info && !that.data.fail_show) {
      that.onNotifyChange()
      that.getDeviceInfo()
    }
    if (that.data.id) {
      that.info(that.data.isFirst);
    }
  },
  info(is_connect) {
    var that = this;
    if (is_connect) {
      wx.showLoading({
        mask: true,
        title: '加载中...',
      })
    }
    wx.request({
      url: app.globalData.api_url + 'device_info',
      data: {
        id: that.data.id,
        openid: wx.getStorageSync('openid'),
        year: that.data.year
      },
      success: function (res) {
        wx.hideLoading()
        console.log('res', res)
        if (res.data.code == 200) {
          const type_list = [{
            id: 1,
            name: '卧室'
          }, {
            id: 2,
            name: '客厅'
          }, {
            id: 3,
            name: '厨房'
          }, {
            id: 4,
            name: '阳台'
          }, {
            id: 5,
            name: '办公室'
          }, {
            id: 6,
            name: '其它'
          }]
          for (const item of res.data.list) {
            for (const type of type_list) {
              if (type.id === item.type) {
                item.type_title = type.name
              }
            }
          }
          that.setData({
            device_info: res.data.device_info,
            device_list: res.data.list,
          })

          var day = res.data.device_info.is_timing === 1 ? '10000000' : '00000000'
          var strArray = res.data.device_info.is_set.split(',');
          console.log(strArray)
          strArray.forEach(item => {
            if (item) {
              const index = parseInt(item)
              day = day.substr(0, 8 - index) + '1' + day.substr(8 - index + 1)
            }
          });
          getApp().globalData.set_time = day
          that.data.yAxis_data = res.data.record
          that.analysis()
          if (is_connect) {
            that.connect();
            console.log('connect')
          }
        } else {
          wx.hideLoading()
          wx.showModal({
            title: '提示',
            content: '请求错误'
          })
        }
      },
      fail: function (e) {
        wx.hideLoading()
        console.log('错误提示', e)
        wx.showModal({
          title: '错误提示',
          content: '请求失败'
        })
      }
    })
  },
  //设备返回灭蚊数
  info1() {
    var that = this;
    wx.request({
      url: app.globalData.api_url + 'device_info',
      data: {
        id: that.data.id,
        openid: wx.getStorageSync('openid'),
        year: that.data.year
      },
      success: function (res) {
        console.log('res', res)
        if (res.data.code == 200) {
          that.setData({
            device_info: res.data.device_info
          })
        } else {
          wx.showModal({
            title: '提示',
            content: '请求错误'
          })
        }
      },
      fail: function (e) {
        console.log('错误提示', e)
        wx.showModal({
          title: '错误提示',
          content: '请求失败'
        })
      }
    })
  },
  getAnalysis() {
    var that = this;
    if (that.data.year === new Date().getFullYear()) {

      wx.setStorageSync('yAxis_data', [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
      that.analysis();
      var year_diff = that.mylength(that.toHex(Math.abs(that.data.device_year - that.data.year) + 1));
      var sum = that.mylength(that.makeChecksum('52c205' + year_diff));
      that.write('a5 aa ac ' + sum + ' 52 c2 05 ' + year_diff + ' c5 cc ca'); //发送灭蚊数查询
    } else {
      console.log('id:', that.data.device_info.id)
      wx.request({
        url: app.globalData.api_url + 'analysis',
        data: {
          id: that.data.device_info.id,
          year: that.data.year
        },
        success: function (res) {
          // console.log('统计数据：', res)
          that.data.yAxis_data = res.data.record
          // wx.setStorageSync('yAxis_data', res.data.record)
          that.analysis();
        },
        fail: function (e) {
          console.log('错误提示', e)
          that.data.yAxis_data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
          // wx.setStorageSync('yAxis_data', [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
          that.analysis();
          wx.showModal({
            title: '错误提示',
            content: '请求失败'
          })
        }
      })
    }
  },
  //上一年
  del() {
    var that = this;
    console.log('del:', that.data.year_click, ' | ', that.data.year_bottom)
    // if (that.data.year_bottom !== 1) {
    if (that.data.year_click === 0) {
      that.setData({
        year: that.data.year - 1
      })
      that.data.year_click = 1
      setTimeout(function () {
        console.log('setTimeout:', that.data.year_click)
        that.setData({
          year_click: 0
        })
      }, 500)
      console.log('year', that.data.year)
      that.getAnalysis()
    } else {
      wx.showToast({
        title: '请勿连续点击',
        icon: 'none'
      })
    }
  },
  //下一年
  add() {
    var that = this;
    const now_year = new Date().getFullYear()
    if (that.data.year >= now_year) {
      wx.showToast({
        title: '已达最大值',
        icon: 'none'
      })
    } else if (that.data.year_click === 1) {
      wx.showToast({
        title: '请勿连续点击',
        icon: 'none'
      })
    } else {
      that.setData({
        year: that.data.year + 1
      })
      that.data.year_click = 1
      that.getAnalysis()
      setTimeout(function () {
        that.setData({
          year_click: 0
        })
      }, 500)
    }
  },
  //表格
  analysis() {
    // var option = {
    //   series: [{
    //     type: 'line',
    //     smooth: true,
    //     label: {
    //       show: true,
    //       position: 'top',
    //       fontSize: 20,
    //     },
    //     areaStyle: {
    //       color: 'rgba(20,76,247,0.2)'
    //     },
    //     data: this.data.yAxis_data
    //   }]
    // };
    this.data.chartOption.series[0].data = this.data.yAxis_data
    chart.setOption(this.data.chartOption);
  },
  //显示设备列表
  show_device() {
    var that = this;
    that.setData({
      show_list: true
    })
  },
  //切换设备  
  bindDevice(e) {
    console.log('e', e)
    var that = this;
    that.setData({
      id: e.currentTarget.dataset.id,
      show_list: false
    })
    that.info(true);
  },
  tapBack() {
    wx.navigateBack({
      delta: 0,
    })
  },
  //问题反馈
  feedback() {
    wx.navigateTo({
      url: '/pages/feedback/index',
    })
  },

  //定时开启关闭
  open_close() {
    var that = this;
    if (that.data.open_click == 0) {
      that.setData({
        open_click: 1,
      })
      setTimeout(function () {
        that.setData({
          open_click: 0
        })
      }, 300)

      var device_info = that.data.device_info;
      device_info.is_timing = !device_info.is_timing;
      that.setData({
        device_info: device_info
      })
      var day = getApp().globalData.set_time
      day = day.substring(1)
      day = (device_info.is_timing ? '1' : '0') + day
      console.log('二进制值：' + day);
      var S_10 = parseInt(day, 2);
      var is_set = S_10.toString(16);
      console.log('十六进制：' + is_set);

      const weeks = ['周日', '周六', '周五', '周四', '周三', '周二', '周一']
      let open = device_info.is_timing ? '开启' : '未开启'
      let text = ''
      for (var index = 1; index <= 7; index++) {
        if (day.slice(index, index + 1) === '1') {
          text = text + (weeks[index - 1] + '、')
        }
      }
      console.warn('小程序发送定时模式设置\n开启状态：', open, '\n重复二进制:', day, '\n重复:', text, '\n时间范围：' + that.data.device_info.startime, '~', that.data.device_info.endtime)
      var startime = that.data.device_info.startime.split(":");
      var endtime = that.data.device_info.endtime.split(":");
      var p3 = that.mylength(that.toHex(startime[0]));
      var p4 = that.mylength(that.toHex(startime[1]));
      var p5 = that.mylength(that.toHex(endtime[0]));
      var p6 = that.mylength(that.toHex(endtime[1]));
      var sum1 = that.mylength(that.makeChecksum('57c2409899' + is_set + p3 + p4 + p5 + p6));
      const send = 'a5 aa ac ' + sum1 + ' 57 c2 40 98 99 ' + is_set + ' ' + p3 + ' ' + p4 + ' ' + p5 + ' ' + p6 + ' c5 cc ca'
      console.warn('发送定时模式命令: ', send)
      that.write(send); //设置主机状态
    } else {
      wx.showToast({
        title: '请勿连续点击',
        icon: 'none'
      })
    }
  },
  updateTiming(is_timing) {
    const that = this
    wx.request({
      url: app.globalData.api_url + 'set_timing',
      data: {
        id: that.data.device_info.id,
        is_timing: that.data.device_info.is_timing ? 1 : 0
      },
      success: function (res) {
        if (res.data.code == 200) {
          wx.showToast({
            title: is_timing ? '定时开启' : '定时关闭',
            icon: 'none'
          })
        } else {
          wx.showModal({
            title: '提示',
            content: '请求错误'
          })
        }
      },
      fail: function (e) {
        console.log('错误提示', e)
        wx.showModal({
          title: '错误提示',
          content: '请求失败'
        })
      }
    })
  },
  //设置定时
  setTime() {
    var that = this;
    var device_id = that.data.device_id;
    wx.navigateTo({
      url: '/pages/device/setime?id=' + that.data.id + '&device_id=' + device_id,
    })
  },
  //开关 
  close_open() {
    var that = this;
    if (that.data.Is_OK_NO) {
      that.data.Is_OK_NO = false;
      if (that.data.is_open == 0) {
        var is_open = '01';
      } else {
        var is_open = '00';
      }
      var sum1 = that.mylength(that.makeChecksum('57c203' + is_open));
      that.write('a5 aa ac ' + sum1 + ' 57 c2 03 ' + is_open + ' c5 cc ca'); //设置主机状态
      setTimeout(function () {
        that.data.Is_OK_NO = true;
      }, 300);
    } else {
      wx.showToast({
        title: '请勿连续点击',
        icon: 'none'
      })
    }
  },
  //恢复出厂设置
  reload() {
    var that = this;
    var sum = that.mylength(that.makeChecksum('57c200'));
    that.write('a5 aa ac ' + sum + ' 57 c2 00 c5 cc ca');
  },

  imgLoadFunc(e) {
    var that = this;
    var height = e.detail.height / (e.detail.width / that.data.width) * 0.9;
    console.log('height', height)
    that.setData({
      height: height,
      canvas_height: height + 200 / 375 * that.data.width
    })
    console.log('canvas_height', that.data.canvas_height)
  },
  show_img() {
    var that = this;
    that.setData({
      show_share: true,
    })
  },
  hide_img() {
    var that = this;
    that.setData({
      show_share: false
    })
  },
  cancel() {
    var that = this;
    that.setData({
      fail_show: false
    })
  },
  reback() {
    wx.navigateBack()
  },
  // 蓝牙
  //搜索连接
  connect: function () {
    var that = this;
    wx.showLoading({
      mask: true,
      title: '正在搜索设备',
    })
    that.setData({
      fail_show: false,
      tip: ''
    })
    if (!wx.openBluetoothAdapter) {
      wx.hideLoading()
      wx.showModal({
        title: '提示',
        showCancel: false,
        content: '当前微信版本过低，无法使用该功能，请升级到最新微信版本后重试。'
      })
      that.setData({
        tip: '设备离线，请重连',
        fail_show: true
      })
      return;
    }
    wx.closeBluetoothAdapter({
      success: function (res) {
        console.log('关闭蓝牙模块')
      },
    });
    wx.openBluetoothAdapter({
      success: function (openRes) {
        //五秒内搜索不到就停止
        that.data.mysear = true
        that.data.timer = setTimeout(function () {
          that.stopsear()
        }, 5000);
        that.startSearch();

        wx.onBluetoothAdapterStateChange(function (res) {
          wx.setStorageSync('available', res.available)
          wx.setStorageSync('discovering', res.discovering)
        })
      },
      fail: function (res) {
        wx.hideLoading()
        wx.showModal({
          showcancel: false,
          title: '提示',
          content: '请先打开蓝牙,然后再继续操作',
        })
        that.setData({
          tip: '设备离线，请重连',
          fail_show: true
        })
      }
    })
  },
  //检测蓝牙状态
  getBlueState: function () {
    var that = this;
    wx.getBluetoothAdapterState({
      success: function (res) {
        if (!!res && res.available) { //蓝牙可用    
          console.log("检测蓝牙状态OK")
          that.startSearch();
        }
      }
    })
  },
  //搜索蓝牙设备
  startSearch: function () {
    var that = this;
    wx.showLoading({
      mask: true,
      title: '正在搜索设备',
    })
    wx.startBluetoothDevicesDiscovery({
      services: [],
      success(res) {
        console.log("开始搜索附近蓝牙设备")
        wx.onBluetoothDeviceFound(function (res) {
          var mac = that.buf2hex(res.devices[0].advertisData);
          if (mac == that.data.device_info.mac) {
            wx.hideLoading();
            that.setData({
              device_id: res.devices[0].deviceId,
              mac: mac,
            });
            that.data.mysear = false
            that.stopSearch();
            that.connectDevice();
          }
        });
      },
      fail(e) {
        console.log('startBluetoothDevicesDiscovery', e)
      }
    })
  },
  //连接到设备
  connectDevice: function () {
    var that = this;
    console.log(that)
    wx.showLoading({ // *** 连接超时提示没检测到设备
      mask: true,
      title: '正在连接设备',
    })
    console.log('connectDevice：', that.data.device_id)
    wx.createBLEConnection({
      deviceId: that.data.device_id,
      success(res) {
        wx.getBLEDeviceServices({
          deviceId: that.data.device_id,
          success: (res) => {
            wx.getBLEDeviceCharacteristics({
              deviceId: that.data.device_id,
              serviceId: that.data.serviceId,
              success: (res) => {
                that.openNotify()
                that.data.isFirst = false
                console.log('连接成功')
              }
            })
          }
        })
      },
      fail: function (res) {
        if (that.data.isFirst) {
          that.data.isFirst = false
          that.connectDevice()
        } else {
          wx.showToast({
            icon: 'error',
            title: '连接失败',
          })
          that.setData({
            tip: '设备离线，请重连',
            fail_show: true
          })
          console.log("连接失败:", res)
        }
      }
    })
  },
  // 开启通知
  openNotify: function () {
    var that = this;
    wx.notifyBLECharacteristicValueChange({
      state: true,
      deviceId: that.data.device_id,
      serviceId: that.data.serviceId,
      characteristicId: that.data.notify_id,
      fail: (res) => {
        console.log('openNotify失败:', res)
      },
      success: function (res) {
        that.onNotifyChange(); //接受消息
        //获取设备总数量
        var num_sum = that.mylength(that.makeChecksum('52c205ff'));
        that.write('a5 aa ac ' + num_sum + ' 52 c2 05 ff c5 cc ca');

        setTimeout(function () {
          //发送主机日期时间
          var now = new Date()
          now.setSeconds(now.getSeconds() - 3)
          var year = now.getFullYear();
          var month = now.getMonth() + 1;
          var day = now.getDate();
          var week = now.getDay();
          var hour = now.getHours();
          var mintue = now.getMinutes();
          var second = now.getSeconds();
          console.warn('小程序发送时间校对:' + year + '-' + month + '-' + day + '(星期' + week + ')' + hour + ':' + mintue + ':' + second);
          year = that.mylength(that.toHex(year));
          month = that.mylength(that.toHex(month));
          day = that.mylength(that.toHex(day));
          hour = that.mylength(that.toHex(hour));
          mintue = that.mylength(that.toHex(mintue));
          second = that.mylength(that.toHex(second));
          week = that.mylength(that.toHex(week));
          const send = ' 57 c2 02 ' + year + ' ' + month + ' ' + day + ' ' + hour + ' ' + mintue + ' ' + second + ' ' + week
          var zj_sum = that.mylength(that.makeChecksum(send));
          console.warn('发送时间校对命令: ', 'a5 aa ac ' + zj_sum + send + ' c5 cc ca')
          that.write('a5 aa ac ' + zj_sum + send + ' c5 cc ca'); //发送读取主机时间日期
          //发送默认时间查询

          that.getDeviceInfo()
        }, 300)
        wx.hideLoading();
        wx.showToast({
          title: '连接成功',
          icon: 'success',
          duration: 1000
        })
        that.setData({
          mysear: false,
          fail_show: false,
          tip: '设备已配对，正常运行中'
        })
        wx.onBLEConnectionStateChange(function (res) {
          // 该方法回调中可以用于处理连接意外断开等异常情况
          console.log(`device ${res.deviceId} state has changed, connected: ${res.connected}`)
          if (!res.connected) {
            that.setData({
              tip: '设备离线，请重连',
              fail_show: true
            })
          }
        })
      },
    })
  },

  getDeviceInfo() {
    var that = this;
    setTimeout(function () {
      var sum = that.mylength(that.makeChecksum('52c203'));
      that.write('a5 aa ac ' + sum + ' 52 c2 03 c5 cc ca'); //发送查询设备状态
      setTimeout(function () {
        var sum = that.mylength(that.makeChecksum('52c240'));
        that.write('a5 aa ac ' + sum + ' 52 c2 40 c5 cc ca'); //发送外设模式
        setTimeout(function () {
          var sum = that.mylength(that.makeChecksum('52c20501'));
          that.write('a5 aa ac ' + sum + ' 52 c2 05 01 c5 cc ca'); //发送灭蚊数查询
        }, 300)
      }, 300)
    }, 300)
    setTimeout(function () {
      var sum1 = that.mylength(that.makeChecksum('52c202'));
      that.write('a5 aa ac ' + sum1 + ' 52 c2 02 c5 cc ca'); //发送读取主机时间日期
    }, 1200)
  },
  //监听消息
  onNotifyChange: function () {
    var that = this;
    /* 设备返回值 */
    wx.onBLECharacteristicValueChange(function (res) {
      var result = res.value;
      var hex = that.buf2hex(result);
      console.log('info 返回的值', hex);
      console.log('返回的值', hex.slice(8));

      // console.log('命令类型：',hex.slice(8, 14))
      if (hex.slice(8, 16) == '52c205ff') { //总灭蚊数
        var num = parseInt(hex.slice(16, 20), 16);
        wx.request({
          url: app.globalData.api_url + 'total_num',
          data: {
            id: that.data.device_info.id,
            num: num
          },
          success: function (res) {
            console.log('某年多月份', res)
            that.info(false);
          },
          fail: function (e) {
            console.log('错误提示', e)
          }
        })
      } else if (hex.slice(8, 14) == '52c205' && hex.length > 28) { //灭蚊数
        console.log('某年多月份灭蚊数', hex)
        var year = parseInt(hex.slice(14, 16), 16); //年
        console.log('year', year);
        console.log('device_year', that.data.device_year);
        var year = that.data.device_year + year - 1;
        var month1 = parseInt(hex.slice(16, 20), 16) ? parseInt(hex.slice(16, 20), 16) : 0; //1月数
        var month2 = parseInt(hex.slice(20, 24), 16) ? parseInt(hex.slice(20, 24), 16) : 0; //2月数
        var month3 = parseInt(hex.slice(24, 28), 16) ? parseInt(hex.slice(24, 28), 16) : 0; //3月数
        var month4 = parseInt(hex.slice(28, 32), 16) ? parseInt(hex.slice(28, 32), 16) : 0; //4月数
        var month5 = parseInt(hex.slice(32, 36), 16) ? parseInt(hex.slice(32, 36), 16) : 0; //5月数
        var month6 = parseInt(hex.slice(36, 40), 16) ? parseInt(hex.slice(36, 40), 16) : 0; //6月数
        var month7 = parseInt(hex.slice(40, 44), 16) ? parseInt(hex.slice(40, 44), 16) : 0; //7月数
        var month8 = parseInt(hex.slice(44, 48), 16) ? parseInt(hex.slice(44, 48), 16) : 0; //8月数
        var month9 = parseInt(hex.slice(48, 52), 16) ? parseInt(hex.slice(48, 52), 16) : 0; //9月数
        var month10 = parseInt(hex.slice(52, 56), 16) ? parseInt(hex.slice(52, 56), 16) : 0; //10月数
        var month11 = parseInt(hex.slice(56, 60), 16) ? parseInt(hex.slice(56, 60), 16) : 0; //11月数
        var month12 = parseInt(hex.slice(60, 64), 16) ? parseInt(hex.slice(60, 64), 16) : 0; //12月数

        var yAxis_data = [];
        yAxis_data.push(month1);
        yAxis_data.push(month2);
        yAxis_data.push(month3);
        yAxis_data.push(month4);
        yAxis_data.push(month5);
        yAxis_data.push(month6);
        yAxis_data.push(month7);
        yAxis_data.push(month8);
        yAxis_data.push(month9);
        yAxis_data.push(month10);
        yAxis_data.push(month11);
        yAxis_data.push(month12);
        that.data.yAxis_data = yAxis_data
        // wx.setStorageSync('yAxis_data', yAxis_data)
        that.analysis();
        var month = month1 + ',' + month2 + ',' + month3 + ',' + month4 + ',' + month5 + ',' + month6 + ',' + month7 + ',' + month8 + ',' + month9 + ',' + month10 + ',' + month11 + ',' + month12;
        wx.request({
          url: app.globalData.api_url + 'record',
          data: {
            id: that.data.device_info.id,
            year: year,
            month: month
          },
          success: function (res) {
            // console.log('某年多月份', res)
            that.info(false);
          },
          fail: function (e) {
            console.log('错误提示', e)
          }
        })
      } else if (hex.slice(8, 14) == '52c202') { //主机主机时间日期
        console.log('灭蚊灯日期时间:', hex)
        var year = parseInt(hex.slice(14, 18), 16);
        var month = parseInt(hex.slice(18, 20), 16);
        var day = parseInt(hex.slice(20, 22), 16);
        var hour = parseInt(hex.slice(22, 24), 16);
        var mintue = parseInt(hex.slice(24, 26), 16);
        var second = parseInt(hex.slice(26, 28), 16);
        var week = parseInt(hex.slice(28, 30), 16);
        console.warn('灭蚊灯时间：' + year + '-' + month + '-' + day + '(星期' + week + ')' + hour + ':' + mintue + ':' + second);
      } else if (hex.slice(8, 14) == '53c203' || hex.slice(8, 14) == '57c203' || hex.slice(8, 14) == '52c203') { //主机状态  运行暂停
        console.log('主机状态', hex)
        var is_open = parseInt(hex.slice(14, 16), 16);
        console.log('is_open', is_open)
        that.setData({
          is_open: is_open
        })
        console.log('主机状态', that.data.is_open)
      } else if (hex.slice(8) == '57c240c5ccca') {
        that.updateTiming(that.data.device_info.is_timing)
      } else if (hex.slice(8, 14) == '52c240') { //主机外设模式
        console.log('主机外设模式', hex)
        var timing = hex.slice(18, 20)
        // var is_timing = hex.slice(18, 20) == '00' ? false : true;
        var x = timing; //这是一个十六进制的字符串表示
        var y = parseInt(x, 16); //十六进制转为十进制
        var z = y.toString(2); //十进制转为2进制
        var startime = parseInt(hex.slice(20, 22), 16) + ':' + parseInt(hex.slice(22, 24), 16);
        var endtime = parseInt(hex.slice(24, 26), 16) + ':' + parseInt(hex.slice(26, 28), 16);

        const weeks = ['周日', '周六', '周五', '周四', '周三', '周二', '周一']
        let open = '未开启'
        if (z.length === 8) {
          open = '开启'
        }
        var text = ''
        for (var index = 0; index < 8 - z.length; index++) {
          z = '0' + z
        }
        for (var index = 1; index <= 7; index++) {
          if (z.slice(index, index + 1) === '1') {
            text = text + (weeks[index - 1] + '、')
          }
        }
        console.warn('灭蚊灯当前定时模式设置\n开启状态：', open, '\n重复二进制:', z, '\n重复:', text, '\n时间范围：' + startime + ' ~ ' + endtime)
        // console.warn('当前定时模式设置 重复：', z, ' 时间范围：' + startime + ' ~ ' + endtime)

        // console.log()
        // var device_info = that.data.device_info;
        // device_info.is_timing = is_timing;
        // device_info.startime = startime;
        // device_info.endtime = endtime;
        // that.setData({
        //   device_info: device_info
        // })
      } else if (hex.slice(8, 14) == '53c205' && hex.length <= 28) { //某年某月上传：灭蚊计数信息
        var num = parseInt(hex.slice(14, 18), 16);
        console.log('灭蚊数', num)
        if (that.data.year === new Date().getFullYear()) {
          var sum = that.mylength(that.makeChecksum('52c20501'));
          that.write('a5 aa ac ' + sum + ' 52 c2 05 01 c5 cc ca');
        }
        wx.request({
          url: app.globalData.api_url + 'total_num',
          data: {
            id: that.data.device_info.id,
            num: num
          },
          success: function (res) {
            // that.info1();
            console.log('总灭蚊数', res)
          },
          fail: function (e) {
            console.log('错误提示', e)
          }
        })
      } else if (hex.slice(8, 14) == '52c200') { //恢复出厂设置
        wx.request({
          url: app.globalData.api_url + 'factory_set',
          data: {
            id: that.data.device_info.id
          },
          success: function (res) {
            console.log('恢复出厂设置', res)
          },
          fail: function (e) {
            console.log('错误提示', e)
          }
        })
      } else if ((hex.slice(8, 14) == '57c200' || hex.slice(8, 14) == '53c200') && hex.slice(14, 16) != '00') { //恢复出厂设置 写返回
        wx.request({
          url: app.globalData.api_url + 'factory_set',
          data: {
            id: that.data.device_info.id
          },
          success: function (res) {
            console.log('恢复出厂设置', res)
          },
          fail: function (e) {
            console.log('错误提示', e)
          }
        })
      } else if (hex.slice(8, 16) == '57c205ff' || hex.slice(8, 16) == '52c20500') { //数据异常
        var yAxis_data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        // wx.setStorageSync('yAxis_data', yAxis_data)
        that.data.yAxis_data = yAxis_data
        that.analysis();
        console.log('数量', yAxis_data)
        that.setData({
          year_bottom: 1
        })
      }
    })
  },
  write: function (str) {
    var that = this;
    var value = str;
    console.log('发送', value);
    /* 将数值转为ArrayBuffer类型数据 */
    var typedArray = new Uint8Array(value.match(/[\da-f]{2}/gi).map(function (h) {
      return parseInt(h, 16)
    }));
    var buffer = typedArray.buffer;
    wx.writeBLECharacteristicValue({
      deviceId: that.data.device_id,
      serviceId: that.data.serviceId,
      characteristicId: that.data.write_id,
      value: buffer,
      success: function (res) {
        console.log('数据发送成功', res);
      },
      fail: function (res) {
        console.log('调用失败', res);
        /* 调用失败时，再次调用 */
        wx.writeBLECharacteristicValue({
          deviceId: that.data.device_id,
          serviceId: that.data.serviceId,
          characteristicId: that.data.write_id,
          value: buffer,
          success: function (res) {
            console.log('第2次数据发送成功', res);
          },
          fail: function (res) {
            console.log('第2次调用失败', res);
            /* 调用失败时，再次调用 */
            wx.writeBLECharacteristicValue({
              deviceId: that.data.device_id,
              serviceId: that.data.serviceId,
              characteristicId: that.data.write_id,
              value: buffer,
              success: function (res) {
                console.log('第3次数据发送成功', res);
              },
              fail: function (res) {
                clearInterval(that.data.interval);
                console.log('第3次调用失败', res);
                that.setData({
                  fail_show: true
                })
                // wx.showModal({
                //   title: '连接设备失败',
                //   content: '请重新连接'
                // })
              }
            });
          }
        });
      }
    });
  },
  //校验和
  makeChecksum: function (data) {
    var that = this
    data = that.splitStr(data, ' ')
    var iTotal = 0;
    var iLen = data.length;
    var iNum = 0;

    while (iNum < iLen) {
      var s = data.substring(iNum, iNum + 2);
      // console.log('parseInt(parseInt(s, 16))', parseInt(parseInt(s, 16)))
      iTotal += parseInt(parseInt(s, 16));
      iNum = iNum + 2;
    }
    // console.log('iTotal', iTotal)
    var sum = parseInt(iTotal % 256);
    return sum.toString(16)
  },
  //转16进制
  toHex: function (num) {
    var rs = "";
    var temp;
    while (num / 16 > 0) {
      temp = num % 16;
      rs = (temp + "").replace("10", "a").replace("11", "b").replace("12", "c").replace("13", "d").replace("14", "e").replace("15", "f") + rs;
      num = parseInt(num / 16);
    }
    //console.warn(rs);　　//打印
    return rs;
  },
  /* 去除输入框输入的值中的空格 */
  splitStr: function (str, s) {
    var newStr = "";
    var strArray = str.split(s);
    for (var i = 0; i < strArray.length; i++) {
      newStr += strArray[i];
    }
    return newStr;
  },
  //2位长度
  mylength: function (str) {
    var str1 = str
    if (str == 0) {
      var str1 = "00"
    }
    if (str.length == 1 || str.length == 3) {
      var str1 = "0" + str
    }
    return str1
  },
  /* ArrayBuffer类型数据转为16进制字符串 */
  buf2hex: function (buffer) { // buffer is an ArrayBuffer
    return Array.prototype.map.call(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
  },
  //停止搜索
  stopSearch: function () {
    var that = this;
    wx.stopBluetoothDevicesDiscovery({
      success: function (res) {}
    })
  },
  disconnect: function () {
    var that = this;
    wx.closeBLEConnection({
      deviceId: that.data.device_id,
      success: function (res) {
        console.log('断开设备连接');
      }
    });
  },
  //取消搜索
  stopsear: function () {
    var that = this
    var mysear = that.data.mysear
    if (mysear) {
      wx.hideLoading()
      wx.showToast({
        icon: 'error',
        title: '搜索不到设备',
      })
      that.stopSearch();
      that.setData({
        fail_show: true
      })
    }
  },
  saveImage() {
    wx.saveImageToPhotosAlbum({
      filePath: '/img/back.png',
      fail: (res) => {

      },
      success: (res) => {
        this.setData({
          show_share: false
        })
        wx.showToast({
          title: '保存成功',
        })
      }
    })
  },
  closeDeviceList() {
    this.setData({
      show_list: false
    })
  },
  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },
  //隐藏页面断开设备
  onHide: function () {
    var that = this;
  },

  onUnload: function () {
    var that = this;
    console.log('onUnload')
    that.stopSearch();
    that.disconnect();
    // wx.setStorageSync('yAxis_data', [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
    if (that.data.timer) {
      clearTimeout(that.data.timer)
    }
  },
})