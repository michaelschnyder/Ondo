using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Devices;
using Microsoft.Azure.Devices.Shared;
using Microsoft.Extensions.Options;

namespace Ondo.Api.Devices
{
    [Route("api/devices")]
    [ApiController]
    public class DeviceController : ControllerBase
    {
        private readonly AzureConfiguration _azureConfiguration;

        public DeviceController(IOptions<AzureConfiguration> azureConfiguration)
        {
            _azureConfiguration = azureConfiguration.Value;
        }
        [HttpGet]
        public async Task<IEnumerable<DeviceDto>> Get()
        {
            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_azureConfiguration.IoTHubConnectionString);
            var query = registryManager.CreateQuery("SELECT * FROM DEVICES");

            var results = await query.GetNextAsTwinAsync();

            return results.Select(Map);
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
