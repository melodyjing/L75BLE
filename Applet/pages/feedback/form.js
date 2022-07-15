// pages/zixunliuyan/index.js
const app = getApp()
Page({

  /**
   * 页面的初始数据
   */
  data: {
    img_list: [],
    max_images: 5,
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {

  },
  compressedImage(paths) {
    const that = this
    var path = ''
    wx.showLoading({
      mask: true,
      title: '压缩中,请稍等...',
    })
    if (paths.length > 0) {
      path = paths[0]['tempFilePath']
    } else {
      wx.hideLoading()
      return
    }
    wx.getImageInfo({
      src: path,
      success: function (res) {
        let canvasRatio = 1.1;
        let picWidth = res.width //图片原始长宽
        let picHeight = res.height
        if (picWidth <= 1920 && picHeight <= 1080) {
          var img_list = that.data.img_list;
          img_list.push({
            src: path,
            img_src: path,
          })
          that.setData({
            img_list: img_list
          })
          paths.splice(0, 1)
          that.compressedImage(paths)
          return
        }
        while (picWidth > 1920 || picHeight > 1080) { // 保证宽高在400以内
          picWidth = Math.trunc(res.width / canvasRatio)
          picHeight = Math.trunc(res.height / canvasRatio)
          canvasRatio = canvasRatio + 0.1;
        }
        const query = wx.createSelectorQuery()
        query.select('#canvas').fields({
          node: true,
          size: true
        }).exec((res) => {
          try {
            const canvas = res[0].node
            const ctx = canvas.getContext('2d')
            let dpr = wx.getSystemInfoSync().pixelRatio
            //这里是设置css样式的宽高。
            //属性宽高是css样式宽高的dpr倍，兼容不同机型，避免绘制的内容模糊。
            that.setData({
              canvasWidth: picWidth / dpr,
              canvasHeight: picHeight / dpr
            })
            canvas.width = picWidth
            canvas.height = picHeight

            const img = canvas.createImage()
            img.src = path
            var index = path.lastIndexOf(".");
            var suffix = 'jpg'
            if (index >= 1 && path.substring(index + 1, path.length) === 'png') {
              suffix = 'png'
            }
            img.onload = () => {
              ctx.drawImage(img, 0, 0, picWidth, picHeight); //先画出图片
              //延迟600ms，避免部分机型未绘制出图片内容就执行保存操作，导致最终的图片是块白色画板。
              setTimeout(() => {
                wx.canvasToTempFilePath({
                  fileType: suffix,
                  canvas: canvas,
                  destWidth: picWidth,
                  destHeight: picHeight,
                  success: function (res) {
                    var tempFilePath = res.tempFilePath //结果路径
                    var img_list = that.data.img_list;
                    img_list.push({
                      src: tempFilePath,
                      img_src: tempFilePath,
                    })
                    that.setData({
                      img_list: img_list
                    })
                    paths.splice(0, 1)
                    that.compressedImage(paths)
                  }
                }, that)
              }, 600)
            }
          } catch (e) {
            console.log(e)
            paths.splice(0, 1)
            that.compressedImage(paths)
            //异常后的操作
          }
        })
      }
    })
  },
  //上传图片
  upload: function () {
    var that = this
    const count = this.data.max_images - this.data.img_list.length
    if (count === 0) {
      wx.showToast({
        icon: 'none',
        title: '最多' + this.data.max_images + '张图片',
      })
      return
    }
    wx.chooseMedia({
      count: count, //最多可以选择的图片总数  
      mediaType: ['image'],
      sizeType: ['compressed'], // 可以指定是原图还是压缩图，默认二者都有  
      sourceType: ['album', 'camera'], // 可以指定来源是相册还是相机，默认二者都有  
      success(res) {
        console.log('图片上查询', res);
        const tempFilePaths = res.tempFiles
        //启动上传等待中...  
        that.compressedImage(tempFilePaths)
      }
    })
  },

  /**
   * 提交
   */
  feedback: function (e) {
    console.log('e', e)
    var that = this
    var title = e.detail.value.title;
    var contact = e.detail.value.contact;
    var phoneReg = new RegExp('^1(3|4|7|5|8)([0-9]{9})$')
    var telephoneReg = new RegExp('^[0][0-9]{2,3}-{0,1}[0-9]{5,10}$')
    console.log(phoneReg.test(contact))
    if (title.length === 0) {
      wx.showModal({
        showCancel: false,
        title: '提示',
        content: '问题和建议描述不能为空。'
      })
      return
    } else if (contact.length > 0 && (!phoneReg.test(contact) && !telephoneReg.test(contact))) {
      wx.showModal({
        showCancel: false,
        title: '提示',
        content: '联系方式格式错误，需为11位手机号码或带区号座机号。'
      })
      return
    }
    var img_list = that.data.img_list;
    var img_src = '';
    var data = {
      title: title,
      contact: contact,
      img_src: img_src,
      openid: wx.getStorageSync('openid')
    }
    wx.showLoading({
      mask: true,
      title: '提交中...',
    })
    var images = 0
    var length = img_list.length
    if (length === 0) {
      that.submit(data)
    } else {
      for (var i = 0; i < length; i++) {
        wx.uploadFile({
          url: app.globalData.api_url + 'upload',
          filePath: img_list[i]['src'],
          name: 'file',
          success: function (res) {
            var data = JSON.parse(res.data)
            console.log(data)
            if (data.code == 200) {
              img_src += data.img_src + ',';
            }
          },
          fail: (res) => {

          },
          complete: () => {
            images++
            if (length === images) {
              data.img_src = img_src
              that.submit(data)
            }
          }
        })
      }
    }

  },
  submit(data) {
    wx.request({
      url: app.globalData.api_url + 'feedback',
      data: data,
      success: function (res) {
        console.log('提交结果', res)
        if (res.data.code == 200) {
          wx.showToast({
            title: res.data.msg,
          })
          setTimeout(function () {
            wx.navigateBack({});
          }, 1000);
        } else {
          wx.showModal({
            title: '提示',
            content: res.data.msg
          })
        }
      },
      fail: function (e) {
        wx.showModal({
          title: '错误提示',
          content: e.errMsg
        })
      },
      complete: () => {
        wx.hideLoading()
      }
    })
  },
  //删除图片
  del: function (e) {
    var that = this
    var index = e.currentTarget.dataset.index;
    var img_list = that.data.img_list;
    img_list.splice(index, 1);
    that.setData({
      img_list: img_list
    })
  },

  //分享
  onShareAppMessage: function () {

  },
  cancel() {
    wx.navigateBack({
      delta: 0,
    })
  }
})