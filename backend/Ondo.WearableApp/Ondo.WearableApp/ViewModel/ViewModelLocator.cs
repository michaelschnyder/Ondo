using CommonServiceLocator;
using GalaSoft.MvvmLight.Ioc;

namespace Ondo.WearableApp
{
    public class ViewModelLocator
    {
        private static ViewModelLocator _locator;

        /// <summary>
        /// Initializes a new instance of the ViewModelLocator class.
        /// </summary>
        public ViewModelLocator()
        {
            ServiceLocator.SetLocatorProvider(() => SimpleIoc.Default);

            SimpleIoc.Default.Register<DeviceViewModel>();
        }

        public static ViewModelLocator Instance => _locator ?? (_locator = new ViewModelLocator());

        public DeviceViewModel DeviceViewModel => ServiceLocator.Current.GetInstance<DeviceViewModel>();

        public static void Cleanup()
        {
            // TODO Clear the ViewModels
        }
    }
}