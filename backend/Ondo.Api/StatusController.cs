using System.Configuration;
using System.Net;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Options;

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
        public IActionResult Index()
        {
            if (string.IsNullOrWhiteSpace(_azureConfiguration.IoTHubConnectionString))
            {
                return this.Problem("Azure IoT Hub Connection String is empty", null,
                    (int?) HttpStatusCode.InternalServerError, "Configuration Error", null);
            }    

            return this.Ok();
        }

    }

    public class AzureConfiguration
    {
        public string IoTHubConnectionString { get; set; }
    }
}