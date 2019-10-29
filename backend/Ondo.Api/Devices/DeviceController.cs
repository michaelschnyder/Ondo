using System.Collections.Generic;
using Microsoft.AspNetCore.Mvc;

namespace Ondo.Api.Devices
{
    [Route("api/devices")]
    [ApiController]
    public class DeviceController : ControllerBase
    {
        [HttpGet]
        public IEnumerable<DeviceDto> Get()
        {
            return new List<DeviceDto>();
        }

        [HttpGet("{id}")]
        public DeviceDto Get(int id)
        {
            return new DeviceDto();
        }
    }
}
