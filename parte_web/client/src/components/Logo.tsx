
import { Car } from "lucide-react"

interface LogoProps {
  size?: "sm" | "md" | "lg"
  showText?: boolean
}

export function Logo({ size = "md", showText = true }: LogoProps) {
  const sizeClasses = {
    sm: "h-6 w-6",
    md: "h-8 w-8", 
    lg: "h-12 w-12"
  }

  const textSizeClasses = {
    sm: "text-lg",
    md: "text-xl",
    lg: "text-3xl"
  }

  return (
    <div className="flex items-center gap-2">
      <div className="relative">
        <div className="absolute inset-0 bg-primary rounded-lg opacity-20"></div>
        <div className="relative bg-primary text-primary-foreground rounded-lg p-2 flex items-center justify-center">
          <Car className={sizeClasses[size]} />
        </div>
      </div>
      {showText && (
        <span className={`font-bold text-primary ${textSizeClasses[size]}`}>
          sPark
        </span>
      )}
    </div>
  )
}
