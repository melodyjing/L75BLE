Component({
  properties: {
    //属性值可以在组件使用时指定
    isCanDraw: {
      type: Boolean,
      value: false,
      observer(newVal, oldVal) {
        if (!newVal) {
          this.setData({
            sharePath: ''
          })
        }
        newVal && this.drawPic()
      }
    },
    qrcode: {
      type: String,
      value: ""
    },
    kill: {
      type: Number,
      value: 0
    },
    day: {
      type: Number,
      value: 0
    }
  },
  data: {
    imgDraw: {}, //绘制图片的大对象
    sharePath: '', //生成的分享图
    visible: false
  },
  methods: {
    handleClose() {
      this.setData({
        visible: false
      })
      this.triggerEvent('close')
    },
    drawPic() {
      if (this.data.sharePath) { //如果已经绘制过了本地保存有图片不需要重新绘制
        this.setData({
          visible: true
        })
        this.triggerEvent('initData')
        return
      }
      wx.showLoading({
        title: '生成中'
      })
      this.setData({
        imgDraw: {
          width: '750px',
          height: '1200px',
          views: [{
              type: 'image',
              url: 'https://noin.ydsyb123.com/static/share_bg.png',
              css: {
                top: '0px',
                left: '0px',
                width: '750px',
                height: '1200px',
                borderRadius: '30px'
              },
            },
            {
              type: 'text',
              text: '已累计灭蚊',
              css: {
                top: '710px',
                left: '375px',
                fontSize: '26px',
                align: 'center',
                color: '#000000',
                fontWeight: 'bold'
              }
            },
            {
              type: 'text',
              text: "" + this.data.kill,
              css: {
                top: '738px',
                fontSize: '106px',
                left: (this.data.kill < 10 ? '400px' : '375px'),
                align: 'center',
                color: '#254bee',
                fontWeight: 'bold',
              }
            },
            {
              type: 'text',
              text: `NOIN已经安全陪伴您${this.data.day}天`,
              css: {
                top: '890px',
                left: '375px',
                align: 'center',
                fontSize: '22px',
                color: '#282828',
                fontWeight: 'bold'
              }
            },
            {
              type: 'text',
              text: `无蚊生活由此开始`,
              css: {
                top: '930px',
                left: '375px',
                align: 'center',
                fontSize: '22px',
                color: '#282828',
                fontWeight: 'bold'
              }
            },{
              type:'rect',
              css: {
                top: '972px',
                left: '275px',
                width: '215px',
                height: '215px',
                borderRadius: '30px',
                color:'#FFFFFF'
              }
            },
            {
              type: 'image',
              url: 'https://noin.ydsyb123.com/static/QR.png',
              css: {
                top: '982px',
                left: '283px',
                width: '196px',
                height: '196px'
              }
            }
          ]
        }
      })
    },
    onClose() {
      this.setData({
        visible: false
      })
      this.triggerEvent('close')
    },
    onImgErr(e) {
      wx.hideLoading()
      wx.showToast({
        title: '生成分享图失败，请刷新页面重试'
      })
    },
    onImgOK(e) {
      wx.hideLoading()
      this.setData({
        sharePath: e.detail.path,
        visible: true,
      })
      //通知外部绘制完成，重置isCanDraw为false
      this.triggerEvent('initData')
    },
    preventDefault() {},
    // 保存图片
    handleSavePhoto() {
      wx.showLoading({
        title: '正在保存...',
        mask: true
      })
      wx.saveImageToPhotosAlbum({
        filePath: this.data.sharePath,
        success: () => {
          wx.showToast({
            title: '保存成功'
          })
          setTimeout(() => {
            this.setData({
              visible: false
            })
            this.triggerEvent('close')
          }, 300)
        },
        fail: () => {
          wx.getSetting({
            success: res => {
              let authSetting = res.authSetting
              if (!authSetting['scope.writePhotosAlbum']) {
                wx.showModal({
                  title: '提示',
                  content: '您未开启保存图片到相册的权限，请点击确定去开启权限！',
                  success(res) {
                    if (res.confirm) {
                      wx.openSetting()
                    }
                  }
                })
              }
            }
          })
          setTimeout(() => {
            wx.hideLoading()
            this.setData({
              visible: false
            })
            this.triggerEvent('close')
          }, 300)
        }
      })
    }
  }
})