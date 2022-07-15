// pages/device/index.js
const app = getApp();
var amapFile = require('/../../utils/amap-wx.js');
var markersData = {
  key: "cf76675ca8ebedc24c65f338e883c22c" //申请的高德地图key
};
Page({

  /**
   * 页面的初始数据
   */
  data: {
    statusBarHeight: app.globalData.statusBarHeight,
    navHeight: app.globalData.navHeight,
    page: 1,
    list: [],
    show: false,
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
    nickname: 'Adam',
    is_add: false
  },

  /**
   * 再次授权位置
   */
  again_auth: function () {
    var that = this;
    wx.showModal({
      title: '授权地址',
      content: '请确认是否授权地理位置',
      success: function (res) {
        if (res.confirm) {
          wx.openSetting({
            success(res) {
              console.log(res.authSetting)
              if (res.authSetting["scope.userLocation"]) {
                // res.authSetting["scope.userLocation"]为trueb表示用户已同意获得定位信息，此时调用getlocation可以拿到信息
                wx.getLocation({
                  type: 'gcj02', //返回可以用于wx.openLocation的经纬度
                  success: function (res) {
                    var latitude = res.latitude //维度
                    var longitude = res.longitude //经度
                    that.loadCity(latitude, longitude);
                    console.log('定位', res)
                  }
                })
              }
            }
          })
        } else {
          that.again_auth();
        }
      }
    })
  },
  /**
   * 获取更新定位
   */
  updateAddress: function () {
    var that = this;
    wx.getSetting({
      success(res) {
        console.log('定位res', res);
        if (res.authSetting['scope.userLocation'] != undefined && res.authSetting['scope.userLocation'] != true) {
          // that.again_auth();
        } else {
          wx.getLocation({
            type: 'gcj02', //返回可以用于wx.openLocation的经纬度
            success: function (res) {
              var latitude = res.latitude //维度
              var longitude = res.longitude //经度
              that.loadCity(latitude, longitude);
              console.log('定位', res)
            },
            fail: res => {
              console.log('获取定位失败', res)
              // that.again_auth();
            }
          })
        }
      }
    })
  },
  //把当前位置的经纬度传给高德地图，调用高德API获取当前地理位置，天气情况等信息
  loadCity: function (latitude, longitude) {
    var that = this;
    var myAmapFun = new amapFile.AMapWX({
      key: markersData.key
    });
    myAmapFun.getRegeo({
      location: '' + longitude + ',' + latitude + '', //location的格式为'经度,纬度'
      success: function (data) {
        console.log('data', data);
        var data = data[0];
        var address = data.regeocodeData.formatted_address
        wx.request({
          url: app.globalData.api_url + 'saveCustom',
          data: {
            address: address,
            openid: wx.getStorageSync('openid'),
            type: 2
          },
          success: function (res) {
            if (res.data.code == 200) {

            } else {
              // wx.showModal({
              //   title: '提示',
              //   content: '请求错误'
              // })
            }
          },
          fail: function (e) {
            console.log('错误提示', e)
            // wx.showModal({
            //   title: '错误提示',
            //   content: '请求失败'
            // })
          }
        })
        console.log('addrress', address);

      },
      fail: function (info) {
        console.log('获取地址', info);
      }
    });
  },

  onShow() {
    var that = this;
    app.getOpenid().then(function (res) {
      if (res.status == 200) {
        that.setData({
          page: 1,
          list: [],
          show: false
        })
        that.updateAddress();
        that.list(1);
      } else {
        console.log(res.data);
      }
    });
  },
  hide() {
    var list = this.data.list;
    for (var i = 0; i < list.length; i++) {
      list[i].selected = false
    }
    this.setData({
      list: list,
      show: false
    })
  },
  //显示设备设置
  show(e) {
    var that = this;
    var index = e.currentTarget.dataset.index;
    var id = e.currentTarget.dataset.id;
    var device_name = e.currentTarget.dataset.device_name;
    var type_id = e.currentTarget.dataset.type_id;
    var list = that.data.list;
    for (var i = 0; i < list.length; i++) {
      list[i].selected = false
    }
    list[index].selected = true;
    that.setData({
      list: list,
      select_index: index,
      select_id: id,
      select_device_name: device_name,
      type_id: type_id,
      show: true
    })
    console.log('select_device_name', that.data.select_device_name)
    console.log('select_type_id', that.data.type_id)
  },

  //设备控制页
  info(e) {
    console.log('e', e)
    var id = e.currentTarget.dataset.id;
    console.log('id', id)
    wx.navigateTo({
      url: '/pages/device/info?id=' + id,
    })
  },

  //添加设备
  add() {
    wx.navigateTo({
      url: '/pages/device/add',
    })
  },
  //移到顶部
  top() {
    var that = this;
    var id = that.data.list[that.data.select_index].id;
    wx.request({
      url: app.globalData.api_url + 'device_sort',
      data: {
        id: id,
        openid: wx.getStorageSync('openid')
      },
      success: function (res) {
        if (res.data.code == 200) {
          that.onShow();
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
  //重命名
  rename() {
    var that = this;
    that.setData({
      show: false,
      is_add: true
    })
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
    var list = that.data.list;
    for (var i = 0; i < list.length; i++) {
      list[i].selected = false
    }
    that.setData({
      is_add: false,
      list: list
    })
  },
  //设备名称
  device_name(e) {
    var that = this;
    that.setData({
      select_device_name: e.detail.value
    })
  },
  //修改设备
  sure_add() {
    var that = this;
    var select_device_name = that.data.select_device_name;
    var type_id = that.data.type_id;
    var id = that.data.select_id;
    console.log('type_id', that.data.type_id)
    console.log('device_name', that.data.select_device_name)
    if (select_device_name && select_device_name != undefined) {
      wx.request({
        url: app.globalData.api_url + 'edit_device',
        data: {
          openid: wx.getStorageSync('openid'),
          id: id,
          name: select_device_name,
          type: type_id
        },
        success: function (res) {
          if (res.data.code == 200) {
            that.setData({
              is_add: false
            })
            wx.showToast({
              title: '修改成功',
              icon: 'none'
            })
            setTimeout(function () {
              that.onShow();
            }, 1000)
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
    } else {
      wx.showToast({
        title: '请输入名称',
        icon: 'none'
      })
    }
  },
  //删除
  del() {
    var that = this;
    var id = that.data.list[that.data.select_index].id;
    wx.request({
      url: app.globalData.api_url + 'del_device',
      data: {
        id: id,
        openid: wx.getStorageSync('openid')
      },
      success: function (res) {
        if (res.data.code == 200) {
          that.setData({
            page: 1,
            list: [],
            show: false
          })
          that.list(1);
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

  //设备列表
  list(page) {
    var that = this;
    wx.request({
      url: app.globalData.api_url + 'index',
      data: {
        page: page,
        openid: wx.getStorageSync('openid')
      },
      success: function (res) {
        if (res.data.code == 200) {
          console.log(res)
          that.setData({
            nickname: res.data.nickname
          })
          if (res.data.list.length > 0) {
            if (page === 1) {
              var list = res.data.list;
            } else {
              var list = that.data.list.concat(res.data.list);
            }
            that.data.page = page
            that.setData({
              list: list
            })
          } else {
            if (that.data.list.length > 0) {
              wx.showToast({
                title: '没有更多数据了',
              })
            }
          }
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

  /**
   * 上拉加载
   */
  onReachBottom: function () {
    var that = this;
    that.list(that.data.page+1);
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})