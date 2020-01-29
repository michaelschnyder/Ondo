using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Azure.Devices.Shared;
using Ondo.Backend.Core.Model;

namespace Ondo.Backend.Core.Services
{
    public interface IDeviceService
    {
        Task<IEnumerable<DeviceDto>> GetDevicesFromAzureAsync();

        Task<Twin> GetDeviceFromAzureAsync(string id);
    }
}
