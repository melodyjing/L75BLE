// pages/device/setime.js
const app = getApp()
Page({

  /**
   * 页面的初始数据
   */
  data: {
    set_checkAll: [{
      id: 8,
      title: '每天',
      checked: true
    }],
    set_list: [{
        id: 1,
        title: '周一',
        checked: false
      },
      {
        id: 2,
        title: '周二',
        checked: false
      },
      {
        id: 3,
        title: '周三',
        checked: false
      },
      {
        id: 4,
        title: '周四',
        checked: false
      },
      {
        id: 5,
        title: '周五',
        checked: false
      },
      {
        id: 6,
        title: '周六',
        checked: false
      },
      {
        id: 7,
        title: '周日',
        checked: false
      }
    ],
    day_All: 0,
    set_list_add: '',
    set_time_id: 8,
    set_time_id1: 8,
    set_time_id_all: 0,
    set_title: '',
    set_title1: '',
    show_form: false,
    show_form1: false,
    is_start: 1,
    startime: '20:00',
    endtime: '08:00',
    startime1: '20:00',
    endtime1: '08:00',
    is_timing: 0,
    mysear: true, //停止搜索用 
    serviceId: '0000FFB0-0000-1000-8000-00805F9B34FB',
    notify_id: '0000FFB2-0000-1000-8000-00805F9B34FB',
    write_id: '0000FFB1-0000-1000-8000-00805F9B34FB',

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    wx.request({
      url: app.globalData.api_url + 'device_info',
      data: {
        id: options.id,
        openid: wx.getStorageSync('openid')
      },
      success: function (res) {
        if (res.data.code == 200) {
          console.log('res', res)
          that.setData({
            set_time_id_all: 0,
            set_title: '',
            id: options.id,
            device_info: res.data.device_info,
            startime: res.data.device_info.startime,
            startime1: res.data.device_info.startime,
            endtime: res.data.device_info.endtime,
            endtime1: res.data.device_info.endtime,
            set_time_id: res.data.device_info.is_set,
            set_time_id1: res.data.device_info.is_set,
            set_time_id_all: res.data.device_info.is_set,
            is_timing: res.data.device_info.is_timing,
          })
          for (var i = 0; i < that.data.set_list.length; i++) {
            if (that.data.set_list[i].id == that.data.set_time_id) {
              that.setData({
                set_title: that.data.set_list[i].title,
                set_title1: that.data.set_list[i].title,
              })
            }
          }
          const set_list = that.data.set_list;
          const set_checkAll = that.data.set_checkAll;
          let checked_num = 0;
          let set_title = that.data.set_title;
          for (let i = 0, lenI = set_list.length; i < lenI; ++i) {
            var set_time_id_all = that.data.set_time_id_all;
            var strArray = set_time_id_all.split(',');
            for (var s_i = 0; s_i < strArray.length; s_i++) {
              if (strArray[s_i] != null && strArray[s_i].length > 0) {
                if (set_list[i].id == strArray[s_i]) {
                  set_list[i].checked = true;
                  set_title = set_title + (set_title.length === 0 ? '' : ',') + set_list[i].title
                  checked_num++
                }
              }
            }
          }
          if (checked_num === 7) {
            set_checkAll[0].checked = true
            set_title = '每天'
          } else {
            set_checkAll[0].checked = false
          }

          that.setData({
            set_list: set_list,
            set_checkAll: set_checkAll,
            set_title: set_title,
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
    var device_id = options.device_id;
    if (device_id) {
      that.setData({
        device_id: device_id
      })
      that.onNotifyChange();
    }
  },
  show_time_form() {
    var that = this;
    that.setData({
      show_form1: true
    })
  },
  save() {
    wx.navigateBack({
      delta: 0,
    })
  },
  // 全选（停用）
  checkAll: function (e) {
    const values = e.detail.value;
    const checked = values.length > 0;
    var set_list = this.data.set_list
    for (var i = 0; i < set_list.length; ++i) {
      set_list[i].checked = checked;
    }
    const checkAll = this.data.set_checkAll
    checkAll[0].checked = checked
    this.setData({
      set_checkAll: checkAll,
      set_list: set_list,
    })
  },
  checkboxChange(e) {
    // console.log('checkbox发生change事件，携带value值为：', e.detail.value)
    const set_list = this.data.set_list;
    const values = e.detail.value;
    const set_checkAll = this.data.set_checkAll;
    for (let all_i = 0, all_lenI = set_checkAll.length; all_i < all_lenI; ++all_i) {
      if (values.length == 7) {
        set_checkAll[all_i].checked = true;
      } else {
        set_checkAll[all_i].checked = false;
      }
    }
    for (let i = 0, lenI = set_list.length; i < lenI; ++i) {
      set_list[i].checked = false;
      for (let j = 0, lenJ = values.length; j < lenJ; ++j) {
        if (set_list[i].id == values[j]) {
          set_list[i].checked = true;
          break;
        }
      }
    }
    this.setData({
      set_list: set_list,
      set_checkAll: set_checkAll
    })
    // console.log('set_checkAll：', this.data.set_checkAll)
  },
  //重复
  select_set(e) {
    // console.log('e', e)
    var that = this;
    that.setData({
      set_title1: e.currentTarget.dataset.time_title,
      set_time_id1: e.currentTarget.dataset.time_id
    })
  },
  cancel1() {
    var that = this;
    that.setData({
      show_form1: false
    })
  },
  sure1() {
    var that = this;
    this.data.updateType = 1
    that.setData({
      set_title: '',
      set_list_add: '',
      day_All: 0
    })
    const set_list = that.data.set_list;
    var selected = []; //当前选择到的值
    // console.log('set_list', set_list);

    for (var i = 0; i < set_list.length; i++) {
      if (set_list[i].checked == true) {
        selected.push(set_list[i]);
      }
    }

    var day = this.data.is_timing === 1 ? '10000000' : '00000000'
    that.data.set_time_id1 = '0';

    for (var i = 0; i < selected.length; i++) {
      that.data.set_title = that.data.set_title + (that.data.set_title.length === 0 ? '' : ',') + selected[i].title
      that.data.set_list_add = selected[i].id + ',' + that.data.set_list_add;
      that.data.set_time_id1 = that.data.set_list_add;
      day = day.substr(0, 8 - selected[i].id) + '1' + day.substr(8 - selected[i].id + 1)
    }
    getApp().globalData.set_time = day

    // console.log('二进制值：' + day);
    var S_10 = parseInt(day, 2);
    var open_status = S_10.toString(16);
    // console.log('十六进制：' + open_status);
    const open = this.data.is_timing === 1 ? '开启' : '未开启'

    console.warn('小程序发送定时模式设置', '\n开启状态: ', open, '\n重复二进制: ', day, '\n重复: ', that.data.set_title, '\n时间范围：', that.data.startime, '~', that.data.endtime)
    if (selected.length === 7) {
      that.data.set_title = '每天'
    }

    // console.log('startime' + that.data.startime);
    // console.log('endtime' + that.data.endtime);
    var startime = that.data.startime.split(":");
    var endtime = that.data.endtime.split(":");
    var p3 = that.mylength(that.toHex(startime[0]));
    var p4 = that.mylength(that.toHex(startime[1]));
    var p5 = that.mylength(that.toHex(endtime[0]));
    var p6 = that.mylength(that.toHex(endtime[1]));
    var sum1 = that.mylength(that.makeChecksum('57 c2 40 98 99' + open_status + p3 + p4 + p5 + p6));
    const send = 'a5 aa ac ' + sum1 + ' 57 c2 40 98 99 ' + open_status + ' ' + p3 + ' ' + p4 + ' ' + p5 + ' ' + p6 + ' c5 cc ca'
    that.write(send); //设置主机状态
    console.warn('发送定时模式命令: ',send)


    wx.showLoading({
      title: '修改中...',
      mask: true
    })
    that.setData({
      show_form1: false
    })
  },
  //时间段弹出框
  select_time() {
    var that = this;
    that.setData({
      show_form: true
    })
  },
  //开启时间
  select_startime(e) {
    var that = this;
    that.setData({
      startime1: e.detail.value
    })
  },
  //关闭时间
  select_endtime(e) {
    var that = this;
    that.setData({
      endtime1: e.detail.value
    })
  },

  //取消
  cancel() {
    var that = this;
    that.setData({
      show_form: false
    })
  },

  //确定
  sure() {
    var that = this;
    this.data.updateType = 2
    that.setData({
      // set_title:'',
      set_list_add: '',
      day_All: 0
    })
    const set_list = that.data.set_list;
    var selected = []; //当前选择到的值
    console.log('set_list', set_list);

    for (var i = 0; i < set_list.length; i++) {
      if (set_list[i].checked == true) {
        selected.push(set_list[i]);
      }
    }

    var day = this.data.is_timing === 1 ? '10000000' : '00000000'
    that.data.set_time_id1 = '0';

    for (var i = 0; i < selected.length; i++) {
      that.data.set_title = that.data.set_title + (that.data.set_title.length === 0 ? '' : ',') + selected[i].title
      that.data.set_list_add = selected[i].id + ',' + that.data.set_list_add;
      that.data.set_time_id1 = that.data.set_list_add;
      day = day.substr(0, 8 - selected[i].id) + '1' + day.substr(8 - selected[i].id + 1)
    }
    console.log('二进制值：' + day);
    getApp().globalData.set_time = day
    var S_10 = parseInt(day, 2);
    var open_status = S_10.toString(16);
    console.log('十六进制：' + open_status);

    const open = this.data.is_timing === 1 ? '开启' : '未开启'
    console.warn('小程序发送定时模式设置', '\n开启状态: ', open, '\n重复二进制: ', day, '\n重复: ', that.data.set_title, '\n时间范围：', that.data.startime, '~', that.data.endtime)
    if (selected.length === 7) {
      that.data.set_title = '每天'
    }
    if (that.data.startime1 != that.data.endtime1) {
      console.log('that.data.startime1', that.data.startime1)
      var startime = that.data.startime1.split(":");
      console.log('startime', startime)
      console.log('that.data.endtime1', that.data.endtime1)
      var endtime = that.data.endtime1.split(":");
      console.log('that.data.startime1', endtime)
      var p3 = that.mylength(that.toHex(startime[0]));
      var p4 = that.mylength(that.toHex(startime[1]));
      var p5 = that.mylength(that.toHex(endtime[0]));
      var p6 = that.mylength(that.toHex(endtime[1]));
      var sum1 = that.mylength(that.makeChecksum('57 c2 40 98 99' + open_status + p3 + p4 + p5 + p6));
      const send = 'a5 aa ac ' + sum1 + ' 57 c2 40 98 99 ' + open_status + ' ' + p3 + ' ' + p4 + ' ' + p5 + ' ' + p6 + ' c5 cc ca'
      that.write(send); //设置主机状态
    console.warn('发送定时模式命令: ',send)

      wx.showLoading({
        title: '修改中...',
        mask: true
      })
      that.setData({
        show_form: false
      })
    } else {
      wx.showModal({
        title: '温馨提示',
        content: '开始时间与结束时间不可一致'
      })
    }
  },

  //监听消息
  onNotifyChange: function () {
    var that = this;
    wx.onBLECharacteristicValueChange(function (res) {
      var result = res.value;
      var hex = that.buf2hex(result);
      console.log('setime 返回的值', hex);
      if (hex.slice(8) == '57c240c5ccca') {
        if (that.data.updateType === 2) {
          wx.request({
            url: app.globalData.api_url + 'timing',
            data: {
              id: that.data.id,
              type: 2,
              startime: that.data.startime1,
              endtime: that.data.endtime1
            },
            success: function (res) {
              console.log('res', res)
              wx.hideLoading()
              if (res.data.code == 200) {
                that.setData({
                  startime: that.data.startime1,
                  endtime: that.data.endtime1,
                  show_form: false
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
              wx.hideLoading()
              wx.showModal({
                title: '错误提示',
                content: '请求失败'
              })
            }
          })
        } else if (that.data.updateType === 1) {
          wx.request({
            url: app.globalData.api_url + 'timing',
            data: {
              id: that.data.id,
              is_set: that.data.set_time_id1
            },
            success: function (res) {
              console.log('set_time_id1:', that.data.set_time_id1)
              console.log(res)
              wx.hideLoading()
              if (res.data.code == 200) {
                that.setData({
                  startime: that.data.startime,
                  endtime: that.data.endtime,
                  set_time_id: that.data.set_time_id1,
                  set_title: that.data.set_title,
                  show_form1: false
                })
              } else {
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
        }
        that.data.updateType = 0
      }
    });
  },
  write: function (str) {
    var that = this;
    var value = str;
    console.log('value', value);
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
                wx.hideLoading()
                wx.showModal({
                  title: '连接设备失败',
                  content: '请重新连接'
                })
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
      that.stopSearch();
      wx.showModal({
        title: '连接不到设备',
        content: '请下拉刷新，重新连接',
      })
    }
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})