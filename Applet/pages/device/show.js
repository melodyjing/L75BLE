const app = getApp()

Page({

  /**
   * 页面的初始数据
   */
  data: {
    device_info:{num:0,day:0}
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that=this
    app.getOpenid().then(function (res) {   
      if (res.status == 200) {
        // scene 需要使用 decodeURIComponent 才能获取到生成二维码时传入的 scene
        console.log('options',options)
        const scene = decodeURIComponent(options.scene);
        if(scene && scene != 'undefined'){
          device_id=scene;
          wx.request({
            url: app.globalData.api_url+'device_info',
            data:{id:device_id,openid:wx.getStorageSync('openid')},
            success:function(res){
              console.log('res',res)
              if(res.data.code == 200){
                that.setData({
                  device_info:res.data.device_info
                })
              }else{
                wx.showModal({
                  title:'提示',
                  content:'请求错误'
                })
              }
            },
            fail:function(e){
              console.log('错误提示',e)
              wx.showModal({
                title: '错误提示',
                content:'请求失败'
              })
            }
          })
        }
      }
    });
  },
  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})