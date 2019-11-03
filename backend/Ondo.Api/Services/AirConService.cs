using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Azure.Devices;
using Microsoft.Extensions.Options;
using Ondo.Api.Helper;
using Ondo.Api.Model;

namespace Ondo.Api.Services
{
    public class AirConService : IAirConService
    {
        private readonly IDeviceService _deviceService;
        private readonly AzureConfiguration _azureConfiguration;
        private readonly AirConMapper _airConMapper;

        public AirConService(IDeviceService deviceService, IOptions<AzureConfiguration> azureConfiguration)
        {
            _deviceService = deviceService;
            _azureConfiguration = azureConfiguration.Value;
            _airConMapper = new AirConMapper();
        }

        public async Task<IEnumerable<AirCon>> GetAirCons()
        {
            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_azureConfiguration.IoTHubConnectionString);
            var query = registryManager.CreateQuery("SELECT * FROM DEVICES");
            var results = await query.GetNextAsTwinAsync();
            return results.Select(x => _airConMapper.GetAirConFromTwin(x));
        }

        public async Task<AirCon> GetAirCon(string deviceId)
        {
            var twin = await _deviceService.GetDeviceFromAzureAsync(deviceId);
            return _airConMapper.GetAirConFromTwin(twin);
        }

        public async Task TurnAirConOn(AirCon airCon)
        {
            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_azureConfiguration.IoTHubConnectionString);
            var twin = await registryManager.GetTwinAsync(airCon.Id);
            twin.Properties.Desired["devicePower"] =  airCon.DevicePower ? 1:0;
            await registryManager.UpdateTwinAsync(airCon.Id, twin, twin.ETag);
        }
    }
}
