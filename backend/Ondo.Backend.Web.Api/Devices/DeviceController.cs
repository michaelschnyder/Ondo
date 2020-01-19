using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Ondo.Backend.Core.Model;
using Ondo.Backend.Core.Services;

namespace Ondo.Api.Devices
{
    [Route("api/devices")]
    [ApiController]
    public class DeviceController : ControllerBase
    {
        private readonly IDeviceService _devicesService;

        public DeviceController(IDeviceService deviceService)
        {
            _devicesService = deviceService;
        }

        [HttpGet]
        public async Task<IEnumerable<DeviceDto>> Get()
        {
            var devices = await _devicesService.GetDevicesFromAzureAsync();
            return devices;
        }
    }
}
