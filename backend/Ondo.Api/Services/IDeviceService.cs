using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Azure.Devices.Shared;
using Ondo.Api.Devices;

namespace Ondo.Api.Services
{
    public interface IDeviceService
    {
        Task<IEnumerable<DeviceDto>> GetDevicesFromAzureAsync();

        Task<Twin> GetDeviceFromAzureAsync(string id);
    }
}
