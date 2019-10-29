using System.Configuration;
using System.Linq;
using System.Net;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Devices;
using Microsoft.Azure.Devices.Shared;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Options;
using Ondo.Api.Devices;

namespace Ondo.Api
{
    [Route("api/status")]
    [ApiController]
    public class StatusController : ControllerBase
    {
        private readonly AzureConfiguration _azureConfiguration;

        public StatusController(IOptions<AzureConfiguration> azureSettings)
        {
            _azureConfiguration = azureSettings.Value;
        }

        [HttpGet]
        public async Task<IActionResult> Index()
        {
            if (string.IsNullOrWhiteSpace(_azureConfiguration.IoTHubConnectionString))
            {
                return this.Problem("Azure IoT Hub Connection String is empty", null,
                    (int?) HttpStatusCode.InternalServerError, "Configuration Error");
            }

            RegistryManager registryManager = RegistryManager.CreateFromConnectionString(_azureConfiguration.IoTHubConnectionString);
            var query = registryManager.CreateQuery("SELECT * FROM DEVICES");

            var results = await query.GetNextAsTwinAsync();

            if (!results.Any())
            {
                this.Problem("Cannot load devices from IoT Hub", null, (int?) HttpStatusCode.InternalServerError);
            }

            return this.Ok();
        }
    }
}