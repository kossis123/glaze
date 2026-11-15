from setuptools import find_packages, setup

package_name = 'robocon_27'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='iamironman',
    maintainer_email='iamironman@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "can_run=robocon_27.can_run:main",
            "run_motor=robocon_27.joy:main",
            "motor=robocon_27.motor_run:main",
            "can_check=robocon_27.can_id_check:main",
            "mr=robocon_27.MR:main",
            "kinematics=robocon_27.kinematics:main"
        ],
    },
)
