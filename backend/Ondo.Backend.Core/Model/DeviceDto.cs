using System;

namespace Ondo.Backend.Core.Model
{
    public class DeviceDto
    {
        public string Ssid { get; set; }
        public string DeviceId { get; set; }
        public string Location { get; set; }
        public string ConnectionStatus { get; set; }
        public object TemperatureC { get; set; }
        public object Humidity { get; set; }
        public string Ip { get; set; }
        public string Rssi { get; set; }
        public DateTime? LastActivityTime { get; set; }
        public DateTime LastUpdateTime { get; set; }
    }
}