/* coulmns.jsx
 *
 * This file defines the columns to be used in PurchasesTable.
*/

//"use client"

export const columns = [
  {
    accessorKey: "coin",
    header: "Coin",
  },
  {
    accessorKey: "quantity",
    header: "Quantity",
  },
  {
    accessorKey: "date",
    header: "Date",
  },
  {
    accessorKey: "time",
    header: "Time",
  },
  {
    accessorKey: "price",
    header: () => <div className="text-right">price</div>,
    cell: ({ row }) => {
      const price = parseFloat(row.getValue("price"))
      const formatted = new Intl.NumberFormat("en-US", {
        style: "currency",
        currency: "USD",
      }).format(price)

      return <div className="text-right font medium">{formatted}</div>
    },
  },
]
