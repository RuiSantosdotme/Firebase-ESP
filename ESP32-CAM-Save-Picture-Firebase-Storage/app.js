// Reference to Picture Storage Path
var imgRef = storageRef.child('data/photo.jpg');

firebase.auth().signInAnonymously().then(function() {

  imgRef.getDownloadURL().then(function(url){
    // `url` is the download URL for 'data/photo.jpg'
    document.querySelector('img').src = url;
    
  }).catch(function(error) {
    console.error(error);
  });
});

imgRef.getMetadata()
  .then((metadata) => {
    console.log(metadata);
    date = new Date(metadata.timeCreated);
    console.log(date.getFullYear()+'-' + (date.getMonth()+1) + '-'+date.getDate());
    console.log(date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds());
    var time = (date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds());
    var writtenDate = (date.getFullYear()+'-' + (date.getMonth()+1) + '-'+date.getDate());
    document.getElementById("date-time").innerHTML = time + " at " + writtenDate;
  })
  .catch((error)=> {
    console.error(error);
  });