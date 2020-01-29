using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Ondo.Backend.Core.Model;
using Ondo.Backend.Core.Services;

namespace Ondo.Api.Model
{
    [Route("api/aircon")]
    [ApiController]
    public class AirconController : ControllerBase
    {
        private readonly IAirConService _airConService;

        public AirconController(IAirConService airConService)
        {
            _airConService = airConService;
        }

        [HttpGet]
        public async Task<IEnumerable<AirCon>> Get()
        {
            var airCons = await _airConService.GetAirCons();
            return airCons;
        }

        [HttpPost]
        public async Task Post(AirCon airCon)
        {
            await _airConService.ChangeAirConSetting(airCon);
        }
    }
}
