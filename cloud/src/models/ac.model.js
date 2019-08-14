var Ac = function (deviceId, status, temperature, quite, powerful, fan) {
    this.fields = {
        deviceId: deviceId || null,
        status: status || null,
        temperature: temperature || null,
        quite: quite || null,
        powerful: powerful || null,
        fan: fan || null
    };
}

module.exports = Ac; 
