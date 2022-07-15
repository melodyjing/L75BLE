// app.js
App({
  onLaunch() {
    //小程序自动更新
    if (wx.canIUse('getUpdateManager')) {
      const updateManager = wx.getUpdateManager()
      updateManager.onCheckForUpdate(function (res) {
        console.log('onCheckForUpdate====', res)
        // 请求完新版本信息的回调
        if (res.hasUpdate) {
          console.log('res.hasUpdate====')
          updateManager.onUpdateReady(function () {
            wx.showModal({
              title: '更新提示',
              content: '新版本已经准备好，是否重启应用？',
              success: function (res) {
                console.log('success====', res)
                // res: {errMsg: "showModal: ok", cancel: false, confirm: true}
                if (res.confirm) {
                  // 新的版本已经下载好，调用 applyUpdate 应用新版本并重启
                  updateManager.applyUpdate()
                }
              }
            })
          })
          updateManager.onUpdateFailed(function () {
            // 新的版本下载失败
            wx.showModal({
              title: '已经有新版本了哟~',
              content: '新版本已经上线啦~，请您删除当前小程序，重新搜索打开哟~'
            })
          })
        }
      })
    }
    let menuButtonObject = wx.getMenuButtonBoundingClientRect();
    wx.getSystemInfo({
      success: res => {
        let statusBarHeight = res.statusBarHeight,
          navTop = menuButtonObject.top, //胶囊按钮与顶部的距离
          navHeight = menuButtonObject.height + (menuButtonObject.top - statusBarHeight) * 2; //导航高度
        this.globalData.navHeight = navHeight;
        this.globalData.navTop = navTop;
        this.globalData.windowHeight = res.windowHeight;
        this.globalData.statusBarHeight = statusBarHeight;
      },
      fail(err) {
        console.log(err);
      }
    })
    // 展示本地存储能力
    const logs = wx.getStorageSync('logs') || []
    logs.unshift(Date.now())
    wx.setStorageSync('logs', logs)
  },

  /**
   * 获取openid
   */
  getOpenid: function () {
    // 检查基金模块用户是否已填写底金
    return new Promise((resolve, reject) => {
      const that = this
      if (wx.getStorageSync('openid') && wx.getStorageSync('openid') != 'undefined') {
        var result = {
          status: 200,
          data: wx.getStorageSync('openid')
        }
        resolve(result);
      } else {
        wx.login({
          success: res => {
            console.log('res', res)
            wx.request({
              url: 'https://noin.ydsyb123.com/formal/WechatApi/getOpenid',
              data: {
                'code': res.code
              },
              success: function (res) {
                console.log('res', res)
                wx.setStorageSync('openid', res.data.openid);
                wx.setStorageSync('session_key', res.data.session_key);
                var result = {
                  status: 200,
                  data: res.data.openid
                }
                resolve(result);
              },
              fail: function (e) {
                console.log('getopenid失败e', e)
              }
            })
          }
        })
      }
    })
  },
  globalData: {
    userInfo: null,
    api_url: 'https://noin.ydsyb123.com/formal/WechatApi/',
    set_time:''
  }
})