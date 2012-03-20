function Camera(){
}

Camera.prototype.DestinationType = {
        DATA_URL : 0,                // Return image as base64 encoded string
        FILE_URI : 1                 // Return image file URI
};

Camera.prototype.PictureSourceType = {
    PHOTOLIBRARY : 0,
    CAMERA : 1,
    SAVEDPHOTOALBUM : 2
};


Camera.prototype.EncodingType = {
     JPEG : 0,               // Return JPEG encoded image
     PNG : 1                 // Return PNG encoded image
};



function CameraOptions() {
};

/*
CameraOptions.prototype.magneticHeading = 0;

quality : 75,
                                  destinationType : Camera.DestinationType.DATA_URL,
                                  sourceType : Camera.PictureSourceType.CAMERA,
                                  allowEdit : true,
                                  encodingType: Camera.EncodingType.JPEG,
                                  targetWidth: 100,
                                  targetHeight: 100
*/

Camera.prototype.getPicture = function( successCallback, errorCallback, options ) {
    // Check the callbacks
    if( typeof successCallback !== "function" ) return;
    if( typeof errorCallback !== "function" ) errorCallback = function() {};
    var cameraOptions = new CameraOptions();

    // Call the native function
    var me = this;
    Cordova.exec( function( p_camera ) {
                      successCallback( p_camera );
                  }, errorCallback, "com.cordova.Camera", "getPicture", [ cameraOptions ] );
};
