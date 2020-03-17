using Microsoft.Azure.Devices;
using Microsoft.Azure.Devices.Shared;
using Ondo.Backend.Core.Model;

namespace Ondo.Backend.Core.Helper
{
    public class AirConMapper
    {
        public AirCon GetAirConFromTwin(Twin result)
        {
            var airCon = Map(result);
            return airCon;
        }

        private AirCon Map(Twin twin)
        {
            var airCon = new AirCon
            {
                Id = twin.DeviceId,
            };

            if (twin.ConnectionState.HasValue)
            {
                airCon.IsConnected = twin.ConnectionState.Value == DeviceConnectionState.Connected;
            }

            if (twin.Tags.Contains("location"))
            {
                airCon.Location = twin.Tags["location"].ToString();
            }

            if (twin.Properties.Desired.Contains("devicePower"))
            {
                airCon.DevicePower = twin.Properties.Desired["devicePower"] == 1;
            }
            
            if (twin.Properties.Desired.Contains("quiteOn"))
            {
                airCon.QuietOn = twin.Properties.Desired["quiteOn"] == 1;
            }

            if (twin.Properties.Desired.Contains("powerfulOn"))
            {
                airCon.PowerfulOn = twin.Properties.Desired["powerfulOn"] == 1;
            }

            if (twin.Properties.Desired.Contains("fanMode"))
            {
                airCon.FanMode = int.Parse(twin.Properties.Desired["fanMode"].ToString());
            }

            if (twin.Properties.Desired.Contains("swingVOn"))
            {
                airCon.SwingVOn = twin.Properties.Desired["swingVOn"] == 1;
            }

            if (twin.Properties.Desired.Contains("swingHOn"))
            {
                airCon.SwingHOn = twin.Properties.Desired["swingHOn"] == 1;
            }

            if (twin.Properties.Desired.Contains("targetTempC"))
            {
                airCon.TargetTempC = int.Parse(twin.Properties.Desired["targetTempC"].ToString());
            }

            return airCon;
        }
    }
}