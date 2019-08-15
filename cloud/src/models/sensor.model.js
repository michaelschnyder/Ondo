var Sensor = function (deviceId, tempC, humidity) {
    this.fields = {
        deviceId: deviceId || null,
        humidity: humidity || null,
        tempC: tempC || null
    };
}

module.exports = Sensor; 