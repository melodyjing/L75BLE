// pages/feedback/czzn.js
const app = getApp()
Page({

  /**
   * 页面的初始数据
   */
  data: {
    list: []
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    wx.request({
      url: app.globalData.api_url + 'guide',
      data: {
        type: 2
      },
      success: function (res) {
        console.log('re', res)
        if (res.data.code == 200) {
          that.setData({
            list: res.data.list
          })
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

  detail(e) {
    var that = this;
    console.log('e', e)
    var id = e.currentTarget.dataset.guide_id;
    console.log('id', id)
    wx.navigateTo({
      url: '/pages/feedback/detail?type=1&id=' + id,
    })
  },
})