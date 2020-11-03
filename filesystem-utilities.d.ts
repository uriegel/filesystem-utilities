declare enum DriveType {
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK
}

interface DriveItem {
    name: string
    description: string
    size: number
    type: DriveType 
    isMounted: boolean
}

declare module 'filesystem-utilities' {
    function getDrives(): Promise<DriveItem[]>
}