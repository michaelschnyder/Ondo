using System.Collections.Generic;
using System.Linq;
using Microsoft.Azure.Devices;
using Microsoft.Extensions.Options;
using System.Threading.Tasks;
using Microsoft.Azure.Devices.Shared;
using Ondo.Api.Devices;

namespace Ondo.Api.Services
{
    public class DevicesService : IDeviceService
    {
        private readonly AzureConfiguration _azureConfiguration;

        public DevicesService(IOptions<AzureConfiguration> azureConfiguration)
        {
            _azureConfiguration = azureConfiguration.Value;
        }

        public async Task<IEnumerable<DeviceDto>> GetDevicesFromAzureAsync()
        {
            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_azureConfiguration.IoTHubConnectionString);
            var query = registryManager.CreateQuery("SELECT * FROM DEVICES");

            var results = await query.GetNextAsTwinAsync();
            return results.Select(Map);
        }

        public async Task<Twin> GetDeviceFromAzureAsync(string id)
        {
            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_azureConfiguration.IoTHubConnectionString);
            var query = registryManager.CreateQuery($"SELECT * FROM DEVICES WHERE deviceId = '{id}'");

            var results = await query.GetNextAsTwinAsync();
            return results.FirstOrDefault();
        }

        private static DeviceDto Map(Twin twin)
        {
            var deviceDto = new DeviceDto
            {
                DeviceId = twin.DeviceId,

                ConnectionStatus = twin.ConnectionState.ToString(),
                LastActivityTime = twin.LastActivityTime,
                LastUpdateTime = twin.Properties.Reported.GetMetadata().LastUpdated
            };

            if (twin.Tags.Contains("location"))
            {
                deviceDto.Location = twin.Tags["location"].ToString();
            }

            if (twin.Properties.Reported.Contains("sensor"))
            {
                deviceDto.TemperatureC = float.Parse(twin.Properties.Reported["sensor"]["tempC"].ToString());
                deviceDto.Humidity = float.Parse(twin.Properties.Reported["sensor"]["humidity"].ToString());
            }

            if (twin.Properties.Reported.Contains("network"))
            {
                deviceDto.Ip = twin.Properties.Reported["network"]["ip"].ToString();
                deviceDto.Ssid = twin.Properties.Reported["network"]["ssid"].ToString();
                deviceDto.Rssi = twin.Properties.Reported["network"]["rssi"].ToString();
            }

            return deviceDto;
        }
    }
}
