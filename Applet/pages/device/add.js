const app = getApp();
Page({

  /**
   * 页面的初始数据
   */
  data: {
    is_search: true,
    is_wsd: false,
    is_connect: false,
    is_add: false,
    deviceArray: [],
    type_list: [{
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
    }, ],
    type_id: 1,
    sb_list: [],
    tip: '正在扫描附近的蓝牙设备...',
    click: 0,


    mysear: true, //停止搜索用 
    serviceId: '0000FFB0-0000-1000-8000-00805F9B34FB',
    notifyId: '0000FFB2-0000-1000-8000-00805F9B34FB',
    writeId: '0000FFB1-0000-1000-8000-00805F9B34FB',
    device_name: 'L75'
  },

  //隐藏
  hide_connect() {
    var that = this;
    that.setData({
      is_connect: false
    })
  },

  //连接设备
  connect_device() {
    var that = this;
    that.setData({
      is_connect: false
    })
    that.connectDevice();
  },

  //选择设备类别
  select_type(e) {
    var that = this;
    that.setData({
      type_id: e.currentTarget.dataset.type_id
    })
  },

  //隐藏添加设备
  hide_add() {
    var that = this;
    that.setData({
      is_add: false
    })
  },
  //设备名称
  device_name(e) {
    var that = this;
    that.setData({
      device_name: e.detail.value
    })
  },
  //确定添加设备
  sure_add() {
    var that = this;
    wx.showLoading({
      mask: true,
      title: '添加中...',
    })
    var device_name = that.data.device_name;
    var type_id = that.data.type_id;
    var mac = that.data.mac;
    console.log('type_id', that.data.type_id)
    console.log('device_name', that.data.device_name)
    if (device_name && device_name != undefined) {
      if (that.data.click == 0) {
        that.setData({
          click: 1
        })
        setTimeout(function () {
          that.setData({
            click: 0
          })
        }, 5000)
        wx.request({
          url: app.globalData.api_url + 'add_device',
          data: {
            openid: wx.getStorageSync('openid'),
            type: 2,
            name: device_name,
            type_id: type_id,
            mac: mac
          },
          success: function (res) {
            wx.hideLoading()
            if (res.data.code == 200) {
              wx.showToast({
                title: '添加成功',
                icon: 'none'
              })
              setTimeout(function () {
                wx.navigateBack()
              }, 500)
            } else {
              wx.showModal({
                title: '提示',
                content: res.data.msg
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
      } else {
        wx.showToast({
          title: '请勿连续点击',
          icon: 'none'
        })
      }
    } else {
      wx.showToast({
        title: '请输入名称',
        icon: 'none'
      })
    }
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
    this.scan()
  },
  scan() {
    var that = this;
    that.setData({
      is_search: true,
      is_wsd: false,
      is_connect: false,
      is_add: false,
      deviceArray: [],
      sb_list: [],
      tip: '正在扫描附近的蓝牙设备...',
    })
    wx.request({
      url: app.globalData.api_url + 'add_device',
      data: {
        openid: wx.getStorageSync('openid')
      },
      success: function (res) {
        if (res.data.code == 200) {
          that.setData({
            sb_list: res.data.device_list
          })
          setTimeout(function () {
            that.search();
          }, 500)
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
  search() {
    var that = this;
    var device = [];
    if (!wx.openBluetoothAdapter) {
      wx.showModal({
        title: '提示',
        content: '当前微信版本过低，无法使用该功能，请升级到最新微信版本后重试。'
      })
      that.setData({
        is_search: false
      })
      return;
    }
    wx.closeBluetoothAdapter({
      success: function (res) {
        console.log('关闭蓝牙模块');
        /* 初始化蓝牙适配器 */
        wx.openBluetoothAdapter({
          success: function (res) {
            // wx.showLoading({
            //   title: '正在搜索设备'
            // })
            that.data.timer = setTimeout(function () {
              // wx.hideLoading();
              if (that.data.deviceArray.length <= 0) {
                wx.stopBluetoothDevicesDiscovery({
                  success: function (res) {
                    console.log('停止搜索设备', res)
                  }
                })
                that.disconnect() //断连
                that.setData({
                  is_search: false,
                  is_wsd: true,
                  tip: '未搜索到设备请返回重新搜索'
                })
              }
            }, 5000)
            console.log('初始化蓝牙适配器成功');
            wx.startBluetoothDevicesDiscovery({
              services: [],
              allowDuplicatesKey: false,
              success: function (res) {
                wx.onBluetoothDeviceFound(function (res) {
                  var isok = true
                  var mac = that.buf2hex(res.devices[0].advertisData);
                  console.log('mac', mac)
                  var sb_list = that.data.sb_list;
                  //去重处理
                  for (var i = 0; i < sb_list.length; i++) {
                    console.log('sb_list', sb_list[i].mac)
                    if (sb_list[i].mac == mac) {
                      isok = false
                    }
                  }

                  if (res.devices[0].name.indexOf("YX") > -1 && isok) {
                    if (res.devices[0].name == "") {
                      var temp = {
                        ID: res.devices[0].deviceId,
                        name: "Unknow device",
                        RSSI: res.devices[0].RSSI,
                        mac: mac
                      }
                    } else {
                      var temp = {
                        ID: res.devices[0].deviceId,
                        name: res.devices[0].name,
                        RSSI: res.devices[0].RSSI,
                        mac: mac
                      }
                    }
                    device.push(temp);
                    that.setData({
                      device: device
                    });
                    that.group(that.data.device, 'ID')
                  }
                });
              },
            });
          },
          fail: function (res) {
            wx.showModal({
              showcancel: false,
              title: '提示',
              content: '请先打开蓝牙,然后再继续操作',
            })
            that.setData({
              is_search: false,
              is_wsd: true,
              tip: '请先打开蓝牙,然后再继续操作'
            })
          }
        })

      }
    });
  },

  /* 去重 */
  group: function (data, key) {
    var that = this;
    var obj = {},
      arr = [];
    for (var a = 0; a < data.length; a++) {
      if (!obj[data[a][key]]) {
        obj[data[a][key]] = [data[a]];
        arr.push(data[a]);
        that.setData({
          deviceArray: arr
        });
      } else {
        obj[data[a][key]].push(data[a]);
      }
    }
  },

  onLianTap(e) {
    var that = this;
    wx.stopBluetoothDevicesDiscovery({
      success: function (res) {
        console.log('停止搜索设备', res)
      }
    })
    var deviceid = e.currentTarget.dataset.device_id;
    var mac = e.currentTarget.dataset.mac;
    that.setData({
      device_id: deviceid,
      mac: mac,
      is_search: false,
      is_connect: true
    })
    wx.offBluetoothDeviceFound();
    // that.connectDevice();
  },

  //连接到设备
  connectDevice: function () {
    var that = this;
    wx.createBLEConnection({
      deviceId: that.data.device_id,
      success(res) {
        that.getDeviceService();
        wx.hideLoading()
      },
      fail: function (res) {
        wx.hideLoading()
        that.getDeviceService();
        wx.showToast({
          title: '连接失败',
          icon: 'none',
          duration: 1000
        })
      }
    })
  },
  //搜索设备服务
  getDeviceService: function () {
    var that = this;
    console.log("搜索设备服务")
    wx.getBLEDeviceServices({
      deviceId: that.data.device_id,
      serviceId: that.data.service_id,
      success: function (res) {
        console.log("设备服务", res)
        var services = res.services;
        that.data.service_id = that.data.serviceId
        that.getDeviceCharacter()
      },
      fail: function (res) {
        console.log("设备服务失败", res)
        that.getDeviceCharacter()
      }
    })
  },
  //获取连接设备的所有特征值  
  getDeviceCharacter() {
    let that = this;
    wx.getBLEDeviceCharacteristics({
      deviceId: that.data.device_id,
      serviceId: that.data.service_id,
      success: function (res) {
        // console.log("特征值", res)
        let notify_id, write_id, read_id;
        for (let i = 0; i < res.characteristics.length; i++) {
          let charc = res.characteristics[i];
          if (charc.properties.notify) {
            if (charc.uuid.indexOf('FFB2') > -1) {
              notify_id = charc.uuid;
            }
          }
          if (charc.properties.write) {
            if (charc.uuid.indexOf('FFB1') > -1) {
              write_id = charc.uuid;
            }
          }
          if (charc.properties.read) {
            read_id = charc.uuid;
          }
        }
        if (notify_id != null) {
          that.data.notify_id = notify_id;
        } else {
          that.data.notify_id = that.data.notifyId;

        }
        if (write_id != null) {
          that.data.write_id = write_id;
        } else {
          that.data.write_id = that.data.writeId;
        }
        that.openNotify();
      }
    })
  },
  // 开启通知
  openNotify: function () {
    var that = this;
    wx.notifyBLECharacteristicValueChange({
      state: true,
      deviceId: that.data.device_id,
      serviceId: that.data.service_id,
      characteristicId: that.data.notify_id,
      success: function (res) {
        // that.onNotifyChange(); //接受消息
        
        wx.showToast({
          title: '连接成功',
          icon: 'success',
          duration: 1000
        })
        that.disconnect()
        that.setData({
          is_add: true
        })
      },
    })
  },
  //监听消息
  onNotifyChange: function () {
    var that = this;
    /* 设备返回值 */
    wx.onBLECharacteristicValueChange(function (res) {
      var result = res.value;
      var hex = that.buf2hex(result);
      console.log('返回值', hex)
    })
  },
  //取低八位
  makeChecksum2: function (data) {
    var iTotal = 0;
    var iLen = data.length;
    var iNum = 0;

    while (iNum < iLen) {
      var s = data.substring(iNum, iNum + 2);
      // System.out.println(s);
      iTotal += parseInt(s, 16);
      iNum = iNum + 2;
    }

    /**
     * 用256求余最大是255，即16进制的FF
     */
    var iMod = parseInt(iTotal % 256);

    var sHex = iMod.toString(16);
    iLen = sHex.length;

    if (iLen < 2) {
      sHex = "0" + sHex;
    }

    var iMod = parseInt(iTotal / 256);

    var sHex2 = iMod.toString(16);
    iLen = sHex2.length;

    if (iLen < 2) {
      sHex2 = "0" + sHex2;
    }
    return sHex; //取低八位
  },
  mylength: function (str) {
    if (str == 0) {
      var str1 = "00"
    }
    if (str < 10 && str > 0) {
      var str1 = "0" + str
    }
    return str1
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
      serviceId: that.data.service_id,
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
          serviceId: that.data.service_id,
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
              serviceId: that.data.service_id,
              characteristicId: that.data.write_id,
              value: buffer,
              success: function (res) {
                console.log('第3次数据发送成功', res);
              },
              fail: function (res) {
                console.log('第3次调用失败', res);
              }
            });
          }
        });
      }
    });
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
  //取消搜索
  stopsear: function () {
    var that = this
    var mysear = that.data.mysear
    if (mysear) {
      wx.hideLoading()
      that.stopSearch();
      wx.showToast({
        title: '搜索不到设备',
      })
    }
  },
  onHide: function () {
    var that = this;
    wx.stopBluetoothDevicesDiscovery({
      success: function (res) {
        console.log('停止搜索设备', res)
      }
    })
    that.disconnect() //断连
    if(that.data.timer){
      clearTimeout(that.data.timer)
    }
  },

  onUnload: function () {
    // var that = this;
    // that.disconnect() //断连
  },
})