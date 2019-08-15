var Device = require("../models/device.model");

module.exports = Object.freeze({
    DINING_ROOM: new Device("Ondo-3c71bf3168b1", "DiningRoom"),
    MASTER_ROOM: new Device("Ondo-3c71bf316563", "MasterRoom"),
    GUEST_ROOM: new Device("Ondo-3c71bf316a40", "GuestRoom"),
    //TODO: Define deviceID
    LIVING_ROOM: new Device("Ondo-xyz123", "LivingRoom")
});