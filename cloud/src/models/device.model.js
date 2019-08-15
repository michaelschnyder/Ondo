var Device = function (deviceId, location) {
    this.fields = {
        deviceId: deviceId || null,
        location: location || null
    };
}

module.exports = Device; 