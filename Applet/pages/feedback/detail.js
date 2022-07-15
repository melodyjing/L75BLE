// pages/product_detail/index.js
const app = getApp()

var WxParse = require('/../../wxParse/wxParse.js');

Page({

  /**
   * 页面的初始数据
   */
  data: {

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    var id = options.id
    var type = options.type
    wx.setNavigationBarTitle({
      title: (type === '1') ? '操作指南' : '常见问题'
    })
    console.log('id', id);
    wx.request({
      url: app.globalData.api_url + 'guide_detail',
      data: {
        id: id
      },
      success: function (res) {
        console.log('re', res)
        if (res.data.code == 200) {
          WxParse.wxParse('content', 'html', res.data.guide_info.content, that, 5)
        } else {
          wx.showToast({
            title: res.data.msg,
            icon: 'none'
          })
        }
      },
      fail: function (e) {
        wx.showModal({
          title: '错误提示',
          content: '请求失败'
        })
      }
    })
  },

  //分享
  onShareAppMessage: function () {},
})