// pages/feedback/index.js
const app = getApp();
Page({

  /**
   * 页面的初始数据
   */
  data: {
    list: [],
    page: 1
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    that.setData({
      list: [],
      page: 1
    })
    that.list();
  },

  detail(e) {
    var that = this;
    console.log('e', e)
    var id = e.currentTarget.dataset.guide_id;
    console.log('id', id)
    wx.navigateTo({
      url: '/pages/feedback/detail?id=' + id,
    })
  },

  //操作指南
  czzn() {
    // var that=this;
    // wx.navigateTo({
    //   url: '/pages/feedback/czzn',
    // })
    var that = this;
    wx.showLoading({
      title: '加载中...',
    })
    wx.request({
      url: app.globalData.api_url + 'guide',
      data: {
        type: 2
      },
      success: function (res) {
        console.log('re', res)
        if (res.data.code == 200) {
          // that.setData({
          //   list: res.data.list
          // })
          if (res.data.list.length > 0) {
            wx.navigateTo({
              url: '/pages/feedback/detail?type=1&id=' + res.data.list[0].id,
            })
          }
        } else {
          wx.showToast({
            title: res.data.msg,
            icon: 'none'
          })
        }
        wx.hideLoading()
      },
      fail: function (e) {
        wx.hideLoading()
        wx.showModal({
          title: '错误提示',
          content: '请求失败'
        })
      }
    })
  },
  //反馈问题
  form() {
    var that = this;
    wx.navigateTo({
      url: '/pages/feedback/form',
    })
  },

  //常见问题列表
  list() {
    var that = this;
    wx.request({
      url: app.globalData.api_url + 'guide',
      data: {
        page: that.data.page
      },
      success: function (res) {
        if (res.data.code == 200) {
          if (res.data.list.length > 0) {
            var list = that.data.list.concat(res.data.list);
            that.setData({
              list: list,
              page: (that.data.page + 1)
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
    that.list();
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})