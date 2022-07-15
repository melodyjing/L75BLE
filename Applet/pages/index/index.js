// index.js
// 获取应用实例
const app = getApp()

Page({
  data: {
  },
  onShow(){
    var that=this;
    var userInfo=wx.getStorageSync('userInfo');
    var openid=wx.getStorageSync('openid');
    if(openid){
      if(userInfo){
        wx.reLaunch({
          url: '/pages/device/index',
        })
      }
    }else{
      app.getOpenid().then(function (res) {   
        if (res.status == 200) {
          
        } else {        
          console.log(res.data);
        }
      });
    }
  },
  getUserProfile:function(){
    var that=this;
    wx.getUserProfile({
      desc: '用于完善会员资料', // 声明获取用户个人信息后的用途，后续会展示在弹窗中，请谨慎填写
      success: (res) => {
        console.log('用户信息',res)
        var userInfo=res.userInfo
        that.setData({
          userInfo:userInfo
        })
        wx.setStorageSync('userInfo', userInfo);
        wx.request({
          url: app.globalData.api_url+'saveCustom',
          data: {
            nickname:userInfo.nickName,
            avatar:userInfo.avatarUrl,
            openid:wx.getStorageSync('openid')
          },
          method: "get",
          success: function (res) {
            console.log('res',res)
            if(res.data.code == 200){
              wx.setStorageSync('userInfo', userInfo);
              wx.reLaunch({
                url: '/pages/device/index',
              })
            }else{
              wx.showToast({
                title: res.msg,
              })
            }
          }
          ,fail:function(err){
             wx.showToast({
               title: '程序错误',
             })
          }
        })
      }
    })
  },
})
