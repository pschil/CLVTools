% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/f_s3generics_clvdata.R
\name{subset.clv.data}
\alias{subset.clv.data}
\alias{subset}
\title{Subsetting clv.data}
\usage{
\method{subset}{clv.data}(x, subset, select, sample = c("full", "estimation", "holdout"), ...)
}
\arguments{
\item{x}{\code{clv.data} to subset}

\item{subset}{logical expression indicating rows to keep}

\item{select}{expression indicating columns to keep}

\item{sample}{Name of sample for which transactions should be extracted,}

\item{...}{further arguments passed to \code{data.table::subset}}
}
\value{
A copy of the \code{data.table} of selected transactions. May contain columns \code{Id}, \code{Date}, and \code{Price}.
}
\description{
Returns a subset of the transaction data stored within the given \code{clv.data} object which meet conditions.
The given expression are forwarded to the \code{data.table} of transactions.
Possible rows to subset and select are \code{Id}, \code{Date}, and \code{Price} (if present).
}
\examples{

library(data.table) # for between()
data(cdnow)

clv.cdnow <- clvdata(cdnow,
  date.format="ymd",
  time.unit = "week",
  estimation.split = "1997-09-30")

# all transactions of customer "1"
subset(clv.cdnow, Id=="1")
subset(clv.cdnow, subset = Id=="1")

# all transactions of customer "111" in the estimation period...
subset(clv.cdnow, Id=="111", sample="estimation")
# ... and in the holdout period
subset(clv.cdnow, Id=="111", sample="holdout")

# all transactions of customers "1", "2", and "999"
subset(clv.cdnow, Id \%in\% c("1","2","999"))

# all transactions on "1997-02-16"
subset(clv.cdnow, Date == "1997-02-16")

# all transactions between "1997-02-01" and "1997-02-16"
subset(clv.cdnow, Date >= "1997-02-01" & Date <= "1997-02-16")
# same using data.table's between
subset(clv.cdnow, between(Date, "1997-02-01","1997-02-16"))

# all transactions with a value between 50 and 100
subset(clv.cdnow, Price >= 50 & Price <= 100)
# same using data.table's between
subset(clv.cdnow, between(Price, 50, 100))

# only keep Id of transactions on "1997-02-16"
subset(clv.cdnow, Date == "1997-02-16", "Id")

}
\seealso{
\code{data.table}'s \code{\link[data.table:subset]{subset}}
}
