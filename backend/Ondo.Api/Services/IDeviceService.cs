using System.Collections.Generic;
using System.Threading.Tasks;
using Ondo.Api.Devices;

namespace Ondo.Api.Services
{
    public interface IDeviceService
    {
        Task<IEnumerable<DeviceDto>> GetDevicesFromAzureAsync();
    }
}
