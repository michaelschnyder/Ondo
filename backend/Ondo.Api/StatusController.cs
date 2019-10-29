using Microsoft.AspNetCore.Mvc;

namespace Ondo.Api
{
    [Route("api/status")]
    [ApiController]
    public class StatusController : ControllerBase
    {
        [HttpGet]
        public IActionResult Index()
        {
            return this.Ok();
        }
    }
}