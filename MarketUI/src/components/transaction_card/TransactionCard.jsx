import {
  Card,
  CardAction,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card"

export function TransactionCard({ selectedcoin }) {
  return (
    <Card className="mx-auto w-full">
      <CardHeader>
        <CardTitle> {selectedcoin} </CardTitle>
        <CardDescription>
          Buy or sell {selectedcoin}
        </CardDescription>
      </CardHeader>
      <CardContent>
        <form></form></CardContent></Card>
  )
}
